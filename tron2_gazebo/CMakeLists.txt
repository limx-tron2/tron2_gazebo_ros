cmake_minimum_required(VERSION 3.10)
project(tron2_gazebo)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_package(catkin REQUIRED
  COMPONENTS
  roscpp
  gazebo_dev
  gazebo_ros_control
  hardware_interface
  realtime_tools
  robot_common
  limxsdk_sim
)

catkin_package(
  INCLUDE_DIRS include
  CATKIN_DEPENDS
  roscpp
  gazebo_ros_control
  hardware_interface
  realtime_tools
  robot_common
  limxsdk_sim
)

include_directories(
  include
  ${catkin_INCLUDE_DIRS}
  ${GAZEBO_INCLUDE_DIRS}
  ${CATKIN_DEVEL_PREFIX}/include
)

link_directories(${CATKIN_DEVEL_PREFIX}/lib)

add_library(tron2_hw_sim
  src/Tron2HWSim.cpp
)

add_dependencies(tron2_hw_sim
  ${catkin_EXPORTED_TARGETS}
  limxsdk_sim_target
  limxsdk_sim_include_target
)

target_link_libraries(tron2_hw_sim
  ${catkin_LIBRARIES}
  ${GAZEBO_LIBRARIES}
  limxsdk_sim
)

install(TARGETS tron2_hw_sim
  ARCHIVE DESTINATION ${CATKIN_PACKAGE_LIB_DESTINATION}
  LIBRARY DESTINATION ${CATKIN_PACKAGE_LIB_DESTINATION}
  RUNTIME DESTINATION ${CATKIN_PACKAGE_BIN_DESTINATION}
)

install(DIRECTORY include/${PROJECT_NAME}/
  DESTINATION ${CATKIN_PACKAGE_INCLUDE_DESTINATION}
  FILES_MATCHING PATTERN "*.h"
)

install(FILES tron2_hw_sim_plugins.xml
  DESTINATION ${CATKIN_PACKAGE_SHARE_DESTINATION}
)

catkin_install_python(PROGRAMS
  scripts/hold_base_in_air.py
  scripts/joystick_teleop.py
  DESTINATION ${CATKIN_PACKAGE_BIN_DESTINATION}
)

install(DIRECTORY config launch worlds
  DESTINATION ${CATKIN_PACKAGE_SHARE_DESTINATION}
)
