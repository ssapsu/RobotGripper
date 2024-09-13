#ifndef ROBOT_GRIPPER_CONSTANTS_h
#define ROBOT_GRIPPER_CONSTANTS_h

#define _USE_MATH_DEFINES

#include <math.h>

constexpr double LINK_LENGTH = 28.75;
constexpr double PI = M_PI;
constexpr double RADIUS = 12.7;
constexpr double MIN_ANGULAR_SPEED = 0.0; // 0 mm/s
constexpr double MAX_ANGULAR_SPEED = 50.0; // 50 mm/s
constexpr double MIN_CLOSURE_PERCENTAGE = 0.0; // 0%
constexpr double MAX_CLOSURE_PERCENTAGE = 100.0; // 100%
constexpr int SERVO_PIN = 33; //
constexpr double SERVO_PWM_FREQUENCY = 50; // 50 Hz
constexpr double DELAY_BETWEEN_STEPS = 20000;

#endif
