find_package(GTest REQUIRED)
include_directories(${GTEST_INCLUDE_DIRS})

add_executable(RobotGripperTest RobotGripperTest.cpp)
target_link_libraries(RobotGripperTest GTest::GTest GTest::Main RobotGripper)

add_executable(RobotGripperAngleControlTest RobotGripperAngleControlTest.cpp)
target_link_libraries(RobotGripperAngleControlTest GTest::GTest GTest::Main RobotGripper)
