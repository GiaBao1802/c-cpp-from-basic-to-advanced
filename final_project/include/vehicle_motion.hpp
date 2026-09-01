#pragma once

#include <algorithm>
#include <cmath>
#include <cstddef>

namespace adas::motion {
struct VehicleState {
    double x_m{};
    double y_m{};
    double yaw_rad{};
    double speed_mps{};
};

struct Actuation {
    double acceleration_mps2{};
    double steering_rad{};
};

class KinematicBicycle final {
public:
    explicit KinematicBicycle(double wheelbase_m) : wheelbase_m_{wheelbase_m} {}
    VehicleState step(VehicleState state, Actuation input, double dt_s) const {
        input.acceleration_mps2 = std::clamp(input.acceleration_mps2, -6.0, 3.0);
        input.steering_rad = std::clamp(input.steering_rad, -0.5, 0.5);
        state.speed_mps = std::max(0.0, state.speed_mps + input.acceleration_mps2 * dt_s);
        const double yaw_rate = state.speed_mps / wheelbase_m_ * std::tan(input.steering_rad);
        state.yaw_rad += yaw_rate * dt_s;
        state.x_m += state.speed_mps * std::cos(state.yaw_rad) * dt_s;
        state.y_m += state.speed_mps * std::sin(state.yaw_rad) * dt_s;
        return state;
    }
private:
    double wheelbase_m_;
};

class Pid final {
public:
    Pid(double kp, double ki, double kd, double minimum, double maximum)
        : kp_{kp}, ki_{ki}, kd_{kd}, minimum_{minimum}, maximum_{maximum} {}
    double update(double error, double dt_s) {
        const double derivative = initialized_ ? (error - previous_) / dt_s : 0.0;
        const double candidate_integral = integral_ + error * dt_s;
        const double raw = kp_ * error + ki_ * candidate_integral + kd_ * derivative;
        const double output = std::clamp(raw, minimum_, maximum_);
        if (output == raw) integral_ = candidate_integral;
        previous_ = error;
        initialized_ = true;
        return output;
    }
    void reset() noexcept { integral_ = previous_ = 0.0; initialized_ = false; }
private:
    double kp_, ki_, kd_, minimum_, maximum_;
    double integral_{}, previous_{};
    bool initialized_{};
};

class LongitudinalController final {
public:
    LongitudinalController() : speed_pid_{0.8, 0.15, 0.02, -4.0, 2.0}, gap_pid_{0.5, 0.05, 0.0, -5.0, 1.5} {}
    double update(double ego_speed, double set_speed, double lead_distance,
                  double lead_speed, bool lead_valid, double dt_s) {
        const double speed_request = speed_pid_.update(set_speed - ego_speed, dt_s);
        if (!lead_valid) return speed_request;
        const double desired_gap = 3.0 + 1.8 * ego_speed;
        const double gap_error = lead_distance - desired_gap;
        const double gap_request = gap_pid_.update(gap_error + 0.5 * (lead_speed - ego_speed), dt_s);
        return std::min(speed_request, gap_request);
    }
private:
    Pid speed_pid_;
    Pid gap_pid_;
};

class LateralController final {
public:
    double update(double cross_track_error_m, double heading_error_rad,
                  double speed_mps) const {
        constexpr double gain = 0.8;
        const double correction = std::atan2(gain * cross_track_error_m, speed_mps + 0.5);
        return std::clamp(heading_error_rad + correction, -0.4, 0.4);
    }
};
}
