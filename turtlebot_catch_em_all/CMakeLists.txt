cmake_minimum_required(VERSION 3.8)
project(turtlebot_catch_em_all)

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# find dependencies
find_package(ament_cmake REQUIRED)
find_package(rclcpp REQUIRED)
find_package(geometry_msgs REQUIRED)
find_package(turtlesim REQUIRED)
find_package(std_msgs REQUIRED)

find_package(rosidl_default_generators REQUIRED)

rosidl_generate_interfaces(${PROJECT_NAME}
  "msg/Turtle.msg"
  "msg/TurtleArray.msg"

  "srv/CaughtTurtle.srv"
)
ament_export_dependencies(rosidl_default_runtime)

add_executable(turtlesim_controller src/turtlesim_controller.cpp)
ament_target_dependencies(turtlesim_controller rclcpp geometry_msgs turtlesim std_msgs)

add_executable(spawner_node src/spawner_node.cpp)
ament_target_dependencies(spawner_node rclcpp geometry_msgs turtlesim std_msgs)

rosidl_target_interfaces(spawner_node
  ${PROJECT_NAME} "rosidl_typesupport_cpp")

  rosidl_target_interfaces(turtlesim_controller
  ${PROJECT_NAME} "rosidl_typesupport_cpp")

install(TARGETS
  turtlesim_controller
  spawner_node
  DESTINATION lib/${PROJECT_NAME}
)

# Install launch files
install(DIRECTORY 
  launch
  DESTINATION share/${PROJECT_NAME}
)

if(BUILD_TESTING)
  find_package(ament_lint_auto REQUIRED)
  # the following line skips the linter which checks for copyrights
  # comment the line when a copyright and license is added to all source files
  set(ament_cmake_copyright_FOUND TRUE)
  # the following line skips cpplint (only works in a git repo)
  # comment the line when this package is in a git repo and when
  # a copyright and license is added to all source files
  set(ament_cmake_cpplint_FOUND TRUE)
  ament_lint_auto_find_test_dependencies()
endif()

ament_package()
