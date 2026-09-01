#include "vehicle_motion.hpp"

#include <gtest/gtest.h>

using namespace adas::motion;

TEST(KinematicBicycle, MovesStraightWithZeroSteering)
{
    KinematicBicycle model{2.8};
    const auto result = model.step({0.0, 0.0, 0.0, 10.0}, {0.0, 0.0}, 0.1);
    EXPECT_NEAR(result.x_m, 1.0, 1e-9);
    EXPECT_NEAR(result.y_m, 0.0, 1e-9);
}

TEST(LongitudinalControl, BrakesForUnsafeGap)
{
    LongitudinalController controller;
    EXPECT_LT(controller.update(20.0, 25.0, 10.0, 10.0, true, 0.02), 0.0);
}

class LateralParameterTest : public ::testing::TestWithParam<double> {};

TEST_P(LateralParameterTest, CommandsTowardLaneCenter)
{
    LateralController controller;
    const double offset = GetParam();
    const double command = controller.update(-offset, 0.0, 15.0);
    EXPECT_LT(command * offset, 0.0);
}

INSTANTIATE_TEST_SUITE_P(BothSides, LateralParameterTest,
                         ::testing::Values(-2.0, -0.5, 0.5, 2.0));
