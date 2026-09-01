#pragma once

#include <cstdint>

namespace adas {

enum class SensorId : std::uint8_t { Radar = 1, Camera = 2 };
enum class FeatureState : std::uint8_t { Init, Operational, Degraded, Fault, Stopped };

struct Detection {
    SensorId sensor{};
    std::uint32_t timestamp_ms{};
    float distance_m{};
    float relative_speed_mps{};
    std::uint8_t counter{};
};

struct FusedObject {
    std::uint32_t timestamp_ms{};
    float distance_m{};
    float relative_speed_mps{};
    bool radar_valid{};
    bool camera_valid{};
};

struct Decision {
    std::uint8_t risk{};
    bool valid{};
    float distance_m{};
    FeatureState state{FeatureState::Init};
};
}
