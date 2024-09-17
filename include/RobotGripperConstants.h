#ifndef ROBOT_GRIPPER_CONSTANTS_h
#define ROBOT_GRIPPER_CONSTANTS_h

#define _USE_MATH_DEFINES

#include <math.h>

constexpr double LINK_LENGTH = 28.75;
constexpr double PI = M_PI;
constexpr double RADIUS = 12.7;
const double MIN_SPEED = 1.0;       // Slow speed in mm/s
const double MODERATE_SPEED = 50.0; // Moderate speed in mm/s
const double MAX_SPEED = 100.0;     // Fast speed in mm/s
constexpr double MIN_CLOSURE_PERCENTAGE = 0.0; // 0%
constexpr double MAX_CLOSURE_PERCENTAGE = 100.0; // 100%
constexpr int SERVO_PIN = 33; //
constexpr double SERVO_PWM_FREQUENCY = 50; // 50 Hz
constexpr double DELAY_BETWEEN_STEPS = 30000;

//Fastest Move in DELAY_BETWEEN_STEPS = 20000


#endif
