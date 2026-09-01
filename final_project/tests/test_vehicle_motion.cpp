#include "vehicle_motion.hpp"

#include <cassert>
#include <cmath>

int main()
{
    using namespace adas::motion;
    KinematicBicycle model{2.8};
    const VehicleState initial{0.0, 0.0, 0.0, 10.0};
    const auto straight = model.step(initial, {0.0, 0.0}, 0.1);
    assert(std::abs(straight.x_m - 1.0) < 1e-9);
    assert(std::abs(straight.y_m) < 1e-9);

    Pid pid{1.0, 0.5, 0.0, -2.0, 2.0};
    assert(pid.update(10.0, 0.1) == 2.0);

    LongitudinalController longitudinal;
    const double free_road = longitudinal.update(10.0, 20.0, 100.0, 20.0, false, 0.02);
    assert(free_road > 0.0);
    const double close_lead = longitudinal.update(20.0, 25.0, 10.0, 10.0, true, 0.02);
    assert(close_lead < 0.0);

    LateralController lateral;
    assert(lateral.update(-1.0, 0.0, 10.0) < 0.0);
    assert(lateral.update(1.0, 0.0, 10.0) > 0.0);

    VehicleState ego{0.0, 1.0, 0.0, 12.0};
    for (int i = 0; i < 500; ++i) {
        const double steer = lateral.update(-ego.y_m, -ego.yaw_rad, ego.speed_mps);
        ego = model.step(ego, {0.0, steer}, 0.02);
    }
    assert(std::abs(ego.y_m) < 0.15);
    return 0;
}
