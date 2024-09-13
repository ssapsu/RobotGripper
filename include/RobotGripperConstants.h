#ifndef ROBOT_GRIPPER_CONSTANTS_h
#define ROBOT_GRIPPER_CONSTANTS_h

#define _USE_MATH_DEFINES

#include <math.h>

constexpr double LINK_LENGTH = 30.0;
constexpr double PI = M_PI;
constexpr double RADIUS = 10.0;
constexpr double MIN_ANGULAR_SPEED = 0.0;
constexpr double MAX_ANGULAR_SPEED = 50.0;
constexpr double MIN_CLOSURE_PERCENTAGE = 0.0;
constexpr double MAX_CLOSURE_PERCENTAGE = 100.0;
constexpr int SERVO_PIN = 33;

#endif
