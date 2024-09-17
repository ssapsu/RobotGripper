#include <gtest/gtest.h>
#include "RobotGripper.h"
#include "RobotGripperConstants.h"

constexpr double EPSILON = 0.0001;

TEST(RobotGripperTest, Degree1to2) {
    RobotGripper gripper;
    gripper.setClosurePercentage(100.0);
    EXPECT_NO_THROW(gripper.closeGripper());
    EXPECT_NO_THROW(gripper.openGripper());
}
int main(int argc, char **argv) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
