#include "RobotGripper.h"
#include <gtest/gtest.h>
#include <iostream>

TEST(RobotGripperTest, test0percentClosure)
{
    try
    {
        RobotGripper gripper;
        gripper.setSpeed(18.0);
        gripper.openGripper();
        gripper.closeGripper();

        // Optionally, verify the final position
        double finalPosition = gripper.getClosurePercentage();
        EXPECT_DOUBLE_EQ(finalPosition, 0.0);
    }
    catch (const std::exception &e)
    {
        FAIL() << "Exception thrown: " << e.what();
    }
}

TEST(RobotGripperTest, test25percentClosure)
{
    try
    {
        RobotGripper gripper;
        gripper.setSpeed(18.0);

        gripper.closeGripper();

        // Set a valid speed
        gripper.setSpeed(18.0); // 18.0 mm/s

        // Set target closure percentage
        gripper.setClosurePercentage(25); // 100%

        // Attempt to close the gripper
        gripper.closeGripper();

        // Optionally, verify the final position
        double finalPosition = gripper.getClosurePercentage();
        EXPECT_DOUBLE_EQ(finalPosition, 25.0);
    }
    catch (const std::exception &e)
    {
        FAIL() << "Exception thrown: " << e.what();
    }
}

TEST(RobotGripperTest, test50percentClosure) {
    try {
        RobotGripper gripper;
        gripper.setSpeed(18.0);

        gripper.closeGripper();

        // Set a valid speed
        gripper.setSpeed(18.0); // 18.0 mm/s

        // Set target closure percentage
        gripper.setClosurePercentage(50); // 100%

        // Attempt to close the gripper
        gripper.closeGripper();

        // Optionally, verify the final position
        double finalPosition = gripper.getClosurePercentage();
        EXPECT_DOUBLE_EQ(finalPosition, 50.0);

    }
    catch (const std::exception& e) {
        FAIL() << "Exception thrown: " << e.what();
    }
}

TEST(RobotGripperTest, test75percentClosure) {
    try {
        RobotGripper gripper;
        gripper.setSpeed(18.0);

        gripper.closeGripper();

        gripper.setSpeed(18.0);

        gripper.setClosurePercentage(75);
        gripper.closeGripper();

        // Optionally, verify the final position
        double finalPosition = gripper.getClosurePercentage();
        EXPECT_DOUBLE_EQ(finalPosition, 75.0);
    }
    catch (const std::exception &e)
    {
        FAIL() << "Exception thrown: " << e.what();
    }
}

TEST(RobotGripperTest, test100percentClosure)
{
    try
    {
        RobotGripper gripper;
        gripper.setSpeed(18.0);

        gripper.closeGripper();

        gripper.setSpeed(18.0);

        gripper.setClosurePercentage(100);
        gripper.closeGripper();

        // Optionally, verify the final position
        double finalPosition = gripper.getClosurePercentage();
        EXPECT_DOUBLE_EQ(finalPosition, 100.0);
    }
    catch (const std::exception &e)
    {
        FAIL() << "Exception thrown: " << e.what();
    }
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
