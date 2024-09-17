cmake_minimum_required(VERSION 3.10)
project(RobotGripperProject)

# Enable C++17
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Find Google Test
find_package(GTest REQUIRED)
include_directories(${GTEST_INCLUDE_DIRS})

# Add RobotGripperTest executable
add_executable(RobotGripperTest RobotGripperTest.cpp)
target_link_libraries(RobotGripperTest GTest::GTest GTest::Main RobotGripper)

# Add RobotGripperAngleControlTest executable
add_executable(RobotGripperAngleControlTest RobotGripperAngleControlTest.cpp)
target_link_libraries(RobotGripperAngleControlTest GTest::GTest GTest::Main RobotGripper)

# Add RobotGripperSpeedTest executable
add_executable(RobotGripperSpeedTest RobotGripperSpeedTest.cpp)
target_link_libraries(RobotGripperSpeedTest GTest::GTest GTest::Main RobotGripper)

# Register tests with CTest
add_test(NAME RobotGripperTest COMMAND RobotGripperTest)
add_test(NAME RobotGripperAngleControlTest COMMAND RobotGripperAngleControlTest)
add_test(NAME RobotGripperSpeedTest COMMAND RobotGripperSpeedTest)
