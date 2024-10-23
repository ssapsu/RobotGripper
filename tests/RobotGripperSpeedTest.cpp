#include "RobotGripper.h"
#include "RobotGripperConstants.h"
#include <gtest/gtest.h>
#include <iostream>

// Define speed categories based on your constants
struct SpeedTestCase {
    double speed;
    std::string description;
};

// Define target positions for testing
const double TARGET_CLOSURE_PERCENTAGE = 100.0; // Fully closed
const double TARGET_OPEN_PERCENTAGE = 0.0;      // Fully open

// Fixture for RobotGripper speed tests
class RobotGripperSpeedTest : public ::testing::TestWithParam<SpeedTestCase> {
protected:
    RobotGripper* gripper;

    void SetUp() override {
        try {
            gripper = new RobotGripper();
        }
        catch (const std::exception& e) {
            FAIL() << "Failed to initialize RobotGripper: " << e.what();
        }
    }

    void TearDown() override {
        delete gripper;
    }
};

// Instantiate test cases
INSTANTIATE_TEST_SUITE_P(
    SpeedTests,
    RobotGripperSpeedTest,
    ::testing::Values(
        SpeedTestCase{MIN_SPEED, "Slow"},      // Adjust based on your MIN_SPEED
        SpeedTestCase{MODERATE_SPEED, "Moderate"}, // Adjust based on your calibration
        SpeedTestCase{MAX_SPEED, "Fast"}     // Adjust based on your MAX_SPEED
    )
);

// Parameterized Test
TEST_P(RobotGripperSpeedTest, MoveToClosureAtSpecifiedSpeed) {
    SpeedTestCase testCase = GetParam();

    try {
        // Set speed
        gripper->setSpeed(testCase.speed);
        std::cout << "Testing " << testCase.description << " Speed: " << testCase.speed << " mm/s" << std::endl;
        EXPECT_DOUBLE_EQ(gripper->getSpeed(), testCase.speed) << "Speed was not set correctly for " << testCase.description << " speed.";

        // Set target closure percentage to 100%
        gripper->setClosurePercentage(TARGET_CLOSURE_PERCENTAGE);

        // Attempt to close the gripper
        gripper->closeGripper();

        // Verify the final position
        double finalPosition = gripper->getClosurePercentage();
        EXPECT_DOUBLE_EQ(finalPosition, TARGET_CLOSURE_PERCENTAGE) << "Gripper failed to reach 100% closure at " << testCase.description << " speed.";

        // Optionally, open the gripper and verify
        gripper->setClosurePercentage(TARGET_OPEN_PERCENTAGE); // 0% closure (fully open)
        gripper->openGripper();
        finalPosition = gripper->getClosurePercentage();
        EXPECT_DOUBLE_EQ(finalPosition, TARGET_OPEN_PERCENTAGE) << "Gripper failed to reach 0% closure at " << testCase.description << " speed.";

    }
    catch (const std::exception& e) {
        FAIL() << "Exception thrown during " << testCase.description << " speed test: " << e.what();
    }
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
