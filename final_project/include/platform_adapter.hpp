#pragma once

#include "adas_controller.hpp"
#include "platform_c.h"

#include <array>

namespace adas {
class CPlatformAdapter final {
public:
    explicit CPlatformAdapter(AdasController& controller) : controller_{controller} {
        Platform_RegisterSensorCallback(&CPlatformAdapter::rx_callback, this);
    }
private:
    static void rx_callback(void *context, const SensorPdu *pdu) {
        if ((context != nullptr) && (pdu != nullptr))
            static_cast<CPlatformAdapter *>(context)->on_rx(*pdu);
    }
    void on_rx(const SensorPdu& pdu) {
        if ((pdu.sensor_id < 1u) || (pdu.sensor_id > 2u)) return;
        auto& expected = counters_[pdu.sensor_id - 1u];
        if (seen_[pdu.sensor_id - 1u] && pdu.counter != expected) {
            Platform_DemReport(EVENT_BAD_COUNTER, DEM_FAILED);
        } else {
            Platform_DemReport(EVENT_BAD_COUNTER, DEM_PASSED);
        }
        seen_[pdu.sensor_id - 1u] = true;
        expected = static_cast<std::uint8_t>(pdu.counter + 1u);
        controller_.submit({static_cast<SensorId>(pdu.sensor_id), pdu.timestamp_ms,
                            static_cast<float>(pdu.distance_cm) / 100.0F,
                            static_cast<float>(pdu.relative_speed_cms) / 100.0F,
                            pdu.counter});
    }
    AdasController& controller_;
    std::array<std::uint8_t, 2> counters_{};
    std::array<bool, 2> seen_{};
};
}
