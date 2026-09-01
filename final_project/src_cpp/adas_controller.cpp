#include "adas_controller.hpp"

#include "platform_c.h"
#include "ipc_publisher.hpp"

#include <algorithm>

namespace adas {
AdasController::AdasController(std::unique_ptr<FusionStrategy> fusion,
                               std::shared_ptr<DecisionObserver> observer)
    : fusion_{std::move(fusion)}, observer_{std::move(observer)} {}

AdasController::~AdasController() { stop(); }

void AdasController::start()
{
    bool expected = false;
    if (running_.compare_exchange_strong(expected, true))
        worker_ = std::thread{&AdasController::run, this};
}

void AdasController::stop()
{
    if (running_.exchange(false)) input_.close();
    if (worker_.joinable()) worker_.join();
}

bool AdasController::submit(Detection detection)
{
    if (!input_.try_push(std::move(detection))) {
        dropped_.fetch_add(1u);
        Platform_DemReport(EVENT_QUEUE_OVERFLOW, DEM_FAILED);
        return false;
    }
    return true;
}

Decision AdasController::last_decision() const
{
    std::lock_guard<std::mutex> lock{state_mutex_};
    return last_;
}

void AdasController::run()
{
    auto ipc = make_local_ipc_publisher();
    while (auto item = input_.wait_pop()) {
        if (item->sensor == SensorId::Radar) radar_ = *item;
        else camera_ = *item;
        if (!radar_ || !camera_) continue;
        const auto fused = fusion_->fuse(*radar_, *camera_);
        const auto decision = decide(fused);
        {
            std::lock_guard<std::mutex> lock{state_mutex_};
            last_ = decision;
        }
        const auto distance_cm = static_cast<std::uint16_t>(
            std::clamp(decision.distance_m * 100.0F, 0.0F, 65535.0F));
        Platform_ComSendAdasStatus(decision.risk, decision.valid, distance_cm);
        Platform_NvMWriteRisk(decision.risk);
        (void)ipc->publish(decision);
        if (auto observer = observer_.lock()) observer->on_decision(decision);
    }
}
}
