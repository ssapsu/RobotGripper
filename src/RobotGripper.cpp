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

RobotGripper::RobotGripper(): speed(MIN_SPEED), currentClosurePercentage(MIN_CLOSURE_PERCENTAGE) {
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
    if (speed < MIN_SPEED || speed > MAX_SPEED) {
        throw std::out_of_range("Speed must be between " + std::to_string(MIN_SPEED) + " and " + std::to_string(MAX_SPEED) + " mm/s.");
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
    targetClosurePercentage = percentage;
}

double RobotGripper::getClosurePercentage() const {
    return currentClosurePercentage;
}

void RobotGripper::openGripper() {
    moveToPosition(MIN_CLOSURE_PERCENTAGE);
}

void RobotGripper::closeGripper() {
    moveToPosition(targetClosurePercentage);
}

double RobotGripper::angleToDutyCycleRatio(double targetAngle) {
    return (2.5 + (targetAngle / 180.0) * 10.0) / 100 * 256;
}

double RobotGripper::getAnglefromPercentage(double percentage) {
    double ratio = percentage / 100;
    double xB0 = RADIUS + LINK_LENGTH;
    double xB90 = sqrt(LINK_LENGTH * LINK_LENGTH - RADIUS * RADIUS);
    double deltaXTotal = xB0 - xB90;
    double A = xB0 - ratio * deltaXTotal;
    double numerator = A * A - (LINK_LENGTH * LINK_LENGTH - RADIUS * RADIUS);
    double denominator = 2 * RADIUS * LINK_LENGTH;
    double cosTheta = numerator / denominator;

    if (cosTheta > 1.0) {
        cosTheta = 1.0;
    } else if (cosTheta < -1.0) {
        cosTheta = -1.0;
    }
    double angle = acos(cosTheta) * 180 / PI;
    printf("Percentage: %f\n", percentage);
    printf("Angle: %f\n", angle);
    return angle;
}

void RobotGripper::moveToPosition(double targetPercentage) {
    double currentAngle = getAnglefromPercentage(currentClosurePercentage);
    double targetAngle = getAnglefromPercentage(targetPercentage);
    double currentDutyCycle = angleToDutyCycleRatio(currentAngle);
    double targetDutyCycle = angleToDutyCycleRatio(targetAngle);
    double dutyCycleStep = (targetDutyCycle - currentDutyCycle) / 10.0;
    for (int i = 0; i < 10; i++) {
        currentDutyCycle += dutyCycleStep;
        gpioPWM(SERVO_PIN, currentDutyCycle);
        usleep(DELAY_BETWEEN_STEPS);
    }

    currentClosurePercentage = targetPercentage;
}
