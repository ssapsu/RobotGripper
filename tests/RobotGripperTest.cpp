#include <gtest/gtest.h>
#include "RobotGripper.h"
#include "RobotGripperConstants.h"

constexpr double EPSILON = 0.0001;

TEST(RobotGripperTest, DefaultConstructor) {
    RobotGripper gripper;
    EXPECT_NEAR(gripper.getSpeed(), MIN_SPEED, EPSILON);
    EXPECT_NEAR(gripper.getClosurePercentage(), MIN_CLOSURE_PERCENTAGE, EPSILON);
}

TEST(RobotGripperTest, SetValidSpeed) {
    RobotGripper gripper;

    gripper.setSpeed(MIN_SPEED);
    EXPECT_NEAR(gripper.getSpeed(), MIN_SPEED, EPSILON);

    gripper.setSpeed(MAX_SPEED);
    EXPECT_NEAR(gripper.getSpeed(), MAX_SPEED, EPSILON);

    double midSpeed = (MIN_SPEED + MAX_SPEED) / 2;
    gripper.setSpeed(midSpeed);
    EXPECT_NEAR(gripper.getSpeed(), midSpeed, EPSILON);
}

TEST(RobotGripperTeset, SetInvalidSpeed) {
    RobotGripper gripper;

    EXPECT_THROW(gripper.setSpeed(MIN_SPEED - 1), std::out_of_range);
    EXPECT_THROW(gripper.setSpeed(MAX_SPEED + 1), std::out_of_range);
}

TEST(RobotGripperTest, SetValidClosurePercentage) {
    RobotGripper gripper;

    gripper.setClosurePercentage(MIN_CLOSURE_PERCENTAGE);
    gripper.closeGripper();
    EXPECT_NEAR(gripper.getClosurePercentage(), MIN_CLOSURE_PERCENTAGE, EPSILON);

    gripper.setClosurePercentage(MAX_CLOSURE_PERCENTAGE);
    gripper.closeGripper();
    EXPECT_NEAR(gripper.getClosurePercentage(), MAX_CLOSURE_PERCENTAGE, EPSILON);

    gripper.setClosurePercentage(50.0);
    gripper.closeGripper();
    EXPECT_NEAR(gripper.getClosurePercentage(), 50.0, EPSILON);
}

TEST(RobotGripperTest, AngleToDutyCycleRatio) {
    RobotGripper gripper;

    EXPECT_NEAR(gripper.angleToDutyCycleRatio(0), 6.4, EPSILON);
    EXPECT_NEAR(gripper.angleToDutyCycleRatio(90.0), 19.2, EPSILON);
    EXPECT_NEAR(gripper.angleToDutyCycleRatio(180.0), 32, EPSILON);

    double angle = 90.0;
    double expectedDutyCycle = (2.5 + (angle / 180.0) * 10.0) / 100 * 256;
    EXPECT_NEAR(gripper.angleToDutyCycleRatio(angle), expectedDutyCycle, EPSILON);
}

TEST(RobotGripperTest, GetAngleFromPercentage) {
    RobotGripper gripper;
    double percentage = 50.0;
    double angle = gripper.getAnglefromPercentage(percentage);
    EXPECT_GE(angle, 0.0);
    EXPECT_LE(angle, 180.0);
}

TEST(RobotGripperTest, OpenCloseGripper) {
    RobotGripper gripper;
    gripper.setClosurePercentage(75.0);

    EXPECT_NO_THROW(gripper.openGripper());
    EXPECT_NO_THROW(gripper.getClosurePercentage());
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
