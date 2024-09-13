add_library(RobotGripper RobotGripper.cpp)
target_include_directories(RobotGripper PUBLIC ${CMAKE_SOURCE_DIR}/include)
target_link_libraries(RobotGripper PUBLIC jetgpio)
