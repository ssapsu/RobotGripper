#include "RobotGripperConstants.h"
#include "RobotGripper.h"
#include <cmath>
#include <iostream>

RobotGripper::RobotGripper(): speed(MIN_ANGULAR_SPEED), closurePercentage(MIN_CLOSURE_PERCENTAGE) {
}

void RobotGripper::setSpeed(double speed) {
    if (speed < MIN_ANGULAR_SPEED || speed > MAX_ANGULAR_SPEED) {
        throw std::out_of_range("Speed must be between " + std::to_string(MIN_ANGULAR_SPEED) + " and " + std::to_string(MAX_ANGULAR_SPEED) + " mm/s.");
    }
    this->speed = speed;
}

double RobotGripper::getSpeed() const {
    return speed;
}

void RobotGripper::setClosurePercentage(double percentage) {
    if (percentage < MIN_CLOSURE_PERCENTAGE || percentage > MAX_CLOSURE_PERCENTAGE) {
        throw std::out_of_range("Closure percentage must be between " + std::to_string(MIN_CLOSURE_PERCENTAGE) + " and " + std::to_string(MAX_CLOSURE_PERCENTAGE) + ".");
    }
    closurePercentage = percentage;
}

double RobotGripper::getClosurePercentage() const {
    return closurePercentage;
}

void RobotGripper::openGripper() {
    moveToPosition(MIN_CLOSURE_PERCENTAGE);
}

void RobotGripper::closeGripper() {
    moveToPosition(closurePercentage);
}

double RobotGripper::angleToDutyCycleRatio(double targetAngle) {
    return 2.5 + (targetAngle / 180.0) * 10.0;
}

double RobotGripper::getAnglefromPercentage(double percentage) {
    double x = percentage / 100 * (LINK_LENGTH + RADIUS - sqrt(pow(LINK_LENGTH, 2) - pow(RADIUS, 2))) + sqrt(pow(LINK_LENGTH, 2) - pow(RADIUS, 2));
    double angle = acos((pow(x, 2) + pow(LINK_LENGTH, 2) - pow(RADIUS, 2)) / (2 * x * LINK_LENGTH));
    return angle * 180.0 / PI;
}

void RobotGripper::moveToPosition(double targetPercentage) {
    double currentAngle = getAnglefromPercentage(closurePercentage);
    double targetAngle = getAnglefromPercentage(targetPercentage);
    double currentDutyCycle = angleToDutyCycleRatio(currentAngle);
    double targetDutyCycle = angleToDutyCycleRatio(targetAngle);
    double dutyCycleStep = (targetDutyCycle - currentDutyCycle) / 100.0;
    for (int i = 0; i < 100; i++) {
        currentDutyCycle += dutyCycleStep;
        std::cout << "Moving to position: " << currentDutyCycle << std::endl;
    }
}
