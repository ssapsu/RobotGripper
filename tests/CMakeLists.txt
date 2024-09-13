find_package(GTest REQUIRED)
include_directories(${GTEST_INCLUDE_DIRS})

add_executable(RobotGripperTest RobotGripperTest.cpp)
target_link_libraries(RobotGripperTest GTest::GTest GTest::Main RobotGripper)
