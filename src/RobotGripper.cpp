#include "RobotGripperConstants.h"
#include "RobotGripper.h"
#include <cmath>
#include <iostream>
#include <stdexcept>
#include <jetgpio.h>
#include <unistd.h>
#include <signal.h>

void signalHandler(int signum) {
    gpioTerminate();
    std::cout << "Interrupt signal (" << signal << ") received. Exiting...\n";
    exit(signum);
}

RobotGripper::RobotGripper(): speed(MIN_ANGULAR_SPEED), closurePercentage(MIN_CLOSURE_PERCENTAGE) {
    signal(SIGINT, signalHandler);
    signal(SIGHUP, signalHandler);
    signal(SIGABRT, signalHandler);
    signal(SIGILL, signalHandler);
    signal(SIGSEGV, signalHandler);
    signal(SIGTERM, signalHandler);

    int initResult = gpioInitialise();
    if (initResult < 0) {
        throw std::runtime_error("JETGPIO initialisation failed with error code: " + std::to_string(initResult));
    } else {
        std::cout << "JETGPIO initialised successfully.\n";
        gpioInitialized = true;
    }

    int pwmFreqResult = gpioSetPWMfrequency(SERVO_PIN, SERVO_PWM_FREQUENCY);
    if (pwmFreqResult < 0) {
        gpioTerminate();
        throw std::runtime_error("Failed to set PWM frequency with error code: " + std::to_string(pwmFreqResult));
    }
};

RobotGripper::~RobotGripper() {
    if (gpioInitialized) {
        gpioPWM(SERVO_PIN, 0);
        gpioTerminate();
        std::cout << "JETGPIO terminated.\n";
    }
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
    return (2.5 + (targetAngle / 180.0) * 10.0) / 100 * 256;
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
    double dutyCycleStep = (targetDutyCycle - currentDutyCycle) / 10.0;
    for (int i = 0; i < 10; i++) {
        currentDutyCycle += dutyCycleStep;
        gpioPWM(SERVO_PIN, currentDutyCycle);
        usleep(DELAY_BETWEEN_STEPS);
    }

    closurePercentage = targetPercentage;
}
