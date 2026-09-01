#pragma once

#include "adas_types.hpp"

#include <algorithm>
#include <cmath>
#include <memory>

namespace adas {
class FusionStrategy {
public:
    virtual ~FusionStrategy() = default;
    virtual FusedObject fuse(const Detection& radar, const Detection& camera) const = 0;
};

class ConservativeFusion final : public FusionStrategy {
public:
    FusedObject fuse(const Detection& radar, const Detection& camera) const override {
        const auto delta = radar.timestamp_ms > camera.timestamp_ms
                         ? radar.timestamp_ms - camera.timestamp_ms
                         : camera.timestamp_ms - radar.timestamp_ms;
        const bool synchronized = delta <= 50u;
        return {std::max(radar.timestamp_ms, camera.timestamp_ms),
                std::min(radar.distance_m, camera.distance_m),
                std::min(radar.relative_speed_mps, camera.relative_speed_mps),
                true, synchronized};
    }
};

inline Decision decide(const FusedObject& object)
{
    Decision result;
    result.distance_m = object.distance_m;
    result.valid = object.radar_valid;
    result.state = object.camera_valid ? FeatureState::Operational : FeatureState::Degraded;
    if (!result.valid || !std::isfinite(object.distance_m) || object.distance_m < 0.0F) {
        result.valid = false;
        result.state = FeatureState::Fault;
        return result;
    }
    const float closing_speed = object.relative_speed_mps < 0.0F ? -object.relative_speed_mps : 0.0F;
    const float ttc = closing_speed > 0.1F ? object.distance_m / closing_speed : 999.0F;
    result.risk = ttc < 1.0F ? 3u : (ttc < 2.5F ? 2u : (ttc < 5.0F ? 1u : 0u));
    return result;
}
}
