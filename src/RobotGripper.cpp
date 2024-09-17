#include "RobotGripperConstants.h"
#include "RobotGripper.h"
#include <cmath>
#include <iostream>
#include <stdexcept>
#include <jetgpio.h>
#include <unistd.h>
#include <signal.h>
#include <algorithm> // For std::max and std::clamp

void signalHandler(int signum) {
    gpioTerminate();
    std::cout << "Interrupt signal (" << signum << ") received. Exiting...\n";
    exit(signum);
}

RobotGripper::RobotGripper() : speed(MIN_SPEED), currentClosurePercentage(MIN_CLOSURE_PERCENTAGE) {
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
}

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
    // Validate target percentage
    if (targetPercentage < MIN_CLOSURE_PERCENTAGE || targetPercentage > MAX_CLOSURE_PERCENTAGE) {
        throw std::out_of_range("Target percentage out of range.");
    }

    // Convert percentages to angles
    double currentAngle = getAnglefromPercentage(currentClosurePercentage);
    double targetAngle = getAnglefromPercentage(targetPercentage);

    // Convert angles to duty cycles
    double currentDutyCycle = angleToDutyCycleRatio(currentAngle);
    double targetDutyCycle = angleToDutyCycleRatio(targetAngle);

    // Calculate the difference and direction
    double dutyCycleDifference = targetDutyCycle - currentDutyCycle;
    int direction = (dutyCycleDifference > 0) ? 1 : -1;
    dutyCycleDifference = std::abs(dutyCycleDifference);

    // Determine step size and number of steps based on speed
    const double DUTY_CYCLE_STEP_SIZE = 1.0; // Adjust as needed for smoothness
    int steps = static_cast<int>(std::ceil(dutyCycleDifference / DUTY_CYCLE_STEP_SIZE));
    steps = std::max(steps, 1); // Ensure at least one step

    // Calculate delay between steps to achieve desired speed
    // Prevent division by zero
    if (speed <= 0.0) {
        throw std::runtime_error("Speed must be greater than zero.");
    }

    double totalMovementTime = dutyCycleDifference / speed; // seconds
    useconds_t delayBetweenSteps = static_cast<useconds_t>((totalMovementTime / steps) * 1e6); // microseconds

    // Debug Output
    std::cout << "Moving to position: " << targetPercentage << "%" << std::endl;
    std::cout << "Current Duty Cycle: " << currentDutyCycle << std::endl;
    std::cout << "Target Duty Cycle: " << targetDutyCycle << std::endl;
    std::cout << "Duty Cycle Difference: " << dutyCycleDifference << std::endl;
    std::cout << "Number of Steps: " << steps << std::endl;
    std::cout << "Total Movement Time: " << totalMovementTime << " seconds" << std::endl;
    std::cout << "Delay Between Steps: " << delayBetweenSteps << " microseconds" << std::endl;

    // Step through the duty cycle changes
    for (int i = 0; i < steps; ++i) {
        currentDutyCycle += direction * DUTY_CYCLE_STEP_SIZE;

        // Clamp duty cycle to valid range (assuming 0-255 for PWM)
        currentDutyCycle = std::clamp(currentDutyCycle, 0.0, 255.0);

        // Set PWM and handle potential errors
        int pwmResult = gpioPWM(SERVO_PIN, static_cast<int>(currentDutyCycle));
        if (pwmResult < 0) {
            throw std::runtime_error("Failed to set PWM during movement.");
        }

        // Debug Output for each step
        std::cout << "Step " << (i + 1) << "/" << steps << ": Duty Cycle = " << currentDutyCycle << std::endl;

        // Delay to control movement speed
        usleep(delayBetweenSteps);
    }

    // Ensure final position is set accurately
    int finalPwmResult = gpioPWM(SERVO_PIN, static_cast<int>(targetDutyCycle));
    if (finalPwmResult < 0) {
        throw std::runtime_error("Failed to set final PWM position.");
    }

    // Update the current closure percentage
    currentClosurePercentage = targetPercentage;

    // Debug Output
    std::cout << "Reached target position: " << targetPercentage << "%" << std::endl;
}
