#include "RobotGripper.h"
#include <algorithm> // For std::max and std::clamp
#include <iostream>
#include <stdexcept>
#include <jetgpio.h>
#include <unistd.h>
#include <signal.h>
#include <cmath>

// Signal handler for graceful termination
void signalHandler(int signum) {
    gpioTerminate();
    std::cout << "Interrupt signal (" << signum << ") received. Exiting...\n";
    exit(signum);
}

RobotGripper::RobotGripper() : speed(MIN_SPEED), currentClosurePercentage(MIN_OPEN_PERCENTAGE) {
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
    if (percentage < MIN_OPEN_PERCENTAGE || percentage > MAX_OPEN_PERCENTAGE) {
        throw std::out_of_range("Closure percentage must be between " + std::to_string(MIN_OPEN_PERCENTAGE) + " and " + std::to_string(MAX_OPEN_PERCENTAGE) + ".");
    }
    targetClosurePercentage = percentage;
}

double RobotGripper::getClosurePercentage() const {
    return currentClosurePercentage;
}

void RobotGripper::openGripper() {
    moveToPosition(MAX_OPEN_PERCENTAGE);
}

void RobotGripper::closeGripper() {
    moveToPosition(targetClosurePercentage);
}

double RobotGripper::angleToDutyCycleRatio(double targetAngle) {
    return (2.5 + (targetAngle / 180.0) * 10.0) / 100 * DUTYCYCLEUNIT;
}

double RobotGripper::getAnglefromPercentage(double percentage)
{
    double ratio = percentage / 100.0;
    double x_max = RADIUS + LINK_LENGTH;                              // Slider position at θ = 0°
    double x_min = sqrt(LINK_LENGTH * LINK_LENGTH - RADIUS * RADIUS); // Slider position at θ = 180°
    double delta_x = x_max - x_min;
    double x = x_max - ratio * delta_x;

    // Calculate the crank angle θ for a given slider position x
    double numerator = x * x + RADIUS * RADIUS - LINK_LENGTH * LINK_LENGTH;
    double denominator = 2 * x * RADIUS;
    double cosTheta = numerator / denominator;
    cosTheta = std::clamp(cosTheta, -1.0, 1.0);
    double theta = acos(cosTheta);
    double angle_deg = theta * 180.0 / PI;
    return angle_deg;
}
// Constants
const double MIN_STEP_SIZE = 0.2;      // Smallest step size for slow speed (more steps)
const double MAX_STEP_SIZE = 1.5;      // Largest step size for fast speed (fewer steps)
const useconds_t FASTEST_USLEEP_DELAY = 30000; // Fastest usleep delay (10 ms between steps)

void RobotGripper::moveToPosition(double targetPercentage) {
    // Validate target percentage
    if (targetPercentage < MIN_OPEN_PERCENTAGE || targetPercentage > MAX_OPEN_PERCENTAGE) {
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

    // The slower the speed, the more steps we take (more interpolation)
    // The faster the speed, the fewer steps we take (less interpolation)
    double interpolationFactor = std::clamp(MAX_SPEED / speed, 1.0, 10.0);  // More interpolation when speed is low
    double stepSize = dutyCycleDifference / (25 * interpolationFactor);      // More steps at lower speeds
    int steps = std::max(static_cast<int>(std::ceil(dutyCycleDifference / stepSize)), 1);

    // Fixed delay based on the fastest version
    useconds_t delayBetweenSteps = FASTEST_USLEEP_DELAY;

    // Debugging output
    std::cout << "Moving to position: " << targetPercentage << "%" << std::endl;
    std::cout << "Current Duty Cycle: " << currentDutyCycle << std::endl;
    std::cout << "Target Duty Cycle: " << targetDutyCycle << std::endl;
    std::cout << "Duty Cycle Difference: " << dutyCycleDifference << std::endl;
    std::cout << "Step Size: " << stepSize << std::endl;
    std::cout << "Number of Steps: " << steps << std::endl;
    std::cout << "Fixed Delay Between Steps: " << delayBetweenSteps << " microseconds" << std::endl;

    // Stepwise movement with dynamic step size
    for (int i = 0; i < steps; ++i) {
        currentDutyCycle += direction * stepSize;
        currentDutyCycle = std::clamp(currentDutyCycle, 0.0, 255.0); // Clamp duty cycle to valid range

        // Set the PWM signal for the current step
        int pwmResult = gpioPWM(SERVO_PIN, static_cast<int>(currentDutyCycle));
        if (pwmResult < 0) {
            throw std::runtime_error("Failed to set PWM during movement.");
        }

        // Debug output for each step
        // std::cout << "Step " << (i + 1) << "/" << steps << ": Duty Cycle = " << currentDutyCycle << std::endl;

        // Wait for the delay time between steps (fixed delay)
        usleep(delayBetweenSteps);
    }

    // Ensure final position is reached
    gpioPWM(SERVO_PIN, static_cast<int>(targetDutyCycle));

    // Update the current closure percentage
    currentClosurePercentage = targetPercentage;

    // Debug output for completion
    std::cout << "Reached target position: " << targetPercentage << "%" << std::endl;
}
