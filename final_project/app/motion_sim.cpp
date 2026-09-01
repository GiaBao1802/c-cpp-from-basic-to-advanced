#include "vehicle_motion.hpp"

#include <cmath>
#include <iomanip>
#include <iostream>

int main()
{
    using namespace adas::motion;
    constexpr double dt = 0.02;
    KinematicBicycle vehicle{2.8};
    LongitudinalController longitudinal;
    LateralController lateral;
    VehicleState ego{0.0, 1.5, 0.05, 15.0};
    double lead_x = 45.0;
    constexpr double lead_speed = 12.0;

    std::cout << "time_s,ego_x_m,ego_y_m,ego_speed_mps,gap_m,accel_mps2,steer_rad\n";
    for (std::size_t k = 0u; k < 1000u; ++k) {
        const double time = static_cast<double>(k) * dt;
        lead_x += lead_speed * dt;
        const double gap = lead_x - ego.x_m;
        const double acceleration = longitudinal.update(ego.speed_mps, 22.0, gap, lead_speed, true, dt);
        const double steering = lateral.update(-ego.y_m, -ego.yaw_rad, ego.speed_mps);
        ego = vehicle.step(ego, {acceleration, steering}, dt);
        if ((k % 50u) == 0u)
            std::cout << std::fixed << std::setprecision(3) << time << ',' << ego.x_m << ','
                      << ego.y_m << ',' << ego.speed_mps << ',' << gap << ','
                      << acceleration << ',' << steering << '\n';
    }
    return (std::abs(ego.y_m) < 0.2 && ego.speed_mps <= 13.0) ? 0 : 1;
}
