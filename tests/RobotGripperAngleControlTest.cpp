#include "RobotGripper.h"
#include <gtest/gtest.h>
#include <iostream>

TEST(RobotGripperTest, Degree0to90Test) {
    try {
        RobotGripper gripper;

        // Set a valid speed
        gripper.setSpeed(5.0); // 5.0 mm/s

        // Set target closure percentage
        gripper.setClosurePercentage(100.0); // 100%

        // Attempt to close the gripper
        gripper.closeGripper();

        // Optionally, verify the final position
        double finalPosition = gripper.getClosurePercentage();
        EXPECT_DOUBLE_EQ(finalPosition, 100.0);

    }
    catch (const std::exception& e) {
        FAIL() << "Exception thrown: " << e.what();
    }
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
