// RobotGripper.h
#ifndef ROBOT_GRIPPER_H
#define ROBOT_gripper_H

#include <stdexcept>
#include "RobotGripperConstants.h"

class RobotGripper {
public:
    RobotGripper();
    ~RobotGripper();

    void setSpeed(double speed);
    double getSpeed() const;

    void setClosurePercentage(double percentage);
    double getClosurePercentage() const;

    double angleToDutyCycleRatio(double targetAngle);
    double getAnglefromPercentage(double percentage);

    void openGripper();
    void closeGripper();

private:
    double speed;
    double currentClosurePercentage;
    double targetClosurePercentage;
    bool gpioInitialized;
    void moveToPosition(double targetPercentage);
};

#endif //ROBOT_GRIPPER_H
