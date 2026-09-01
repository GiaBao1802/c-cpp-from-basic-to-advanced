#include "adas_controller.hpp"
#include "platform_adapter.hpp"
#include "platform_c.h"

#include <chrono>
#include <iostream>
#include <memory>
#include <thread>

class ConsoleMetrics final : public adas::DecisionObserver {
public:
    void on_decision(const adas::Decision& decision) override {
        ++count_;
        std::cout << "Decision #" << count_ << " risk=" << unsigned(decision.risk) << '\n';
    }
private:
    unsigned count_{};
};

static void inject(std::uint8_t sensor, std::uint8_t counter, std::uint16_t cm,
                   std::int16_t speed, std::uint32_t time)
{
    const std::uint16_t raw_speed = static_cast<std::uint16_t>(speed);
    const std::uint8_t pdu[10] = {sensor, counter, std::uint8_t(cm), std::uint8_t(cm >> 8),
        std::uint8_t(raw_speed), std::uint8_t(raw_speed >> 8), std::uint8_t(time),
        std::uint8_t(time >> 8), std::uint8_t(time >> 16), std::uint8_t(time >> 24)};
    (void)Platform_InjectRxPdu(pdu, sizeof(pdu));
}

int main()
{
    Platform_Init();
    auto metrics = std::make_shared<ConsoleMetrics>();
    adas::AdasController controller{std::make_unique<adas::ConservativeFusion>(), metrics};
    adas::CPlatformAdapter adapter{controller};
    (void)adapter;
    controller.start();
    Platform_SetMode(ECU_RUN);
    inject(1u, 0u, 1200u, -600, 1000u);
    inject(2u, 0u, 1300u, -550, 1020u);
    std::this_thread::sleep_for(std::chrono::milliseconds{20});
    Platform_SetMode(ECU_POST_RUN);
    controller.stop();
    while (!Platform_NvMMainFunction()) {}
    Platform_SetMode(ECU_SHUTDOWN);
    std::cout << "Persisted risk=" << unsigned(Platform_NvMReadRisk()) << '\n';
    return 0;
}
