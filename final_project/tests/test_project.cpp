#include "adas_controller.hpp"
#include "fusion.hpp"
#include "platform_adapter.hpp"
#include "platform_c.h"

#include <cassert>
#include <chrono>
#include <memory>
#include <thread>

class TestObserver final : public adas::DecisionObserver {
public:
    void on_decision(const adas::Decision&) override { ++calls; }
    unsigned calls{};
};

static void inject(std::uint8_t sensor, std::uint8_t counter)
{
    const std::uint8_t pdu[10] = {sensor, counter, 0xE8u, 0x03u, 0x0Cu, 0xFEu,
                                  0x64u, 0u, 0u, 0u};
    assert(Platform_InjectRxPdu(pdu, sizeof(pdu)));
}

int main()
{
    const adas::Detection radar{adas::SensorId::Radar, 100u, 10.0F, -5.0F, 0u};
    const adas::Detection camera{adas::SensorId::Camera, 120u, 12.0F, -4.0F, 0u};
    adas::ConservativeFusion fusion;
    const auto decision = adas::decide(fusion.fuse(radar, camera));
    assert(decision.valid && decision.risk == 2u);

    Platform_Init();
    Platform_SetMode(ECU_RUN);
    auto observer = std::make_shared<TestObserver>();
    adas::AdasController controller{std::make_unique<adas::ConservativeFusion>(), observer};
    adas::CPlatformAdapter adapter{controller};
    (void)adapter;
    controller.start();
    inject(1u, 0u);
    inject(2u, 0u);
    std::this_thread::sleep_for(std::chrono::milliseconds{30});
    controller.stop();
    assert(observer->calls >= 1u);
    assert(controller.last_decision().valid);

    Platform_NvMMainFunction();
    assert(Platform_NvMReadRisk() > 0u);

    Platform_Init();
    Platform_SetMode(ECU_RUN);
    adas::AdasController counter_controller{std::make_unique<adas::ConservativeFusion>(), observer};
    adas::CPlatformAdapter counter_adapter{counter_controller};
    inject(1u, 0u);
    inject(1u, 2u);
    assert(Platform_DemFailureCount(EVENT_BAD_COUNTER) == 1u);
    return 0;
}
