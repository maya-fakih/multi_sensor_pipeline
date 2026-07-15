cmake_minimum_required(VERSION 3.8)
project(multi_sensor_pipeline)

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()
# find dependencies
find_package(ament_cmake REQUIRED)
find_package(rclcpp REQUIRED)
find_package(multi_sensor_interfaces REQUIRED)
# uncomment the following section in order to fill in
# further dependencies manually.
# find_package(<dependency> REQUIRED)

add_executable(sensor_driver src/sensor_driver.cpp)
ament_target_dependencies(sensor_driver rclcpp multi_sensor_interfaces)

add_executable(filter_node src/filter_node.cpp)
ament_target_dependencies(filter_node rclcpp multi_sensor_interfaces)

add_executable(logger_node src/logger_node.cpp)
ament_target_dependencies(logger_node rclcpp multi_sensor_interfaces)

install(TARGETS 
sensor_driver
filter_node
logger_node
DESTINATION lib/${PROJECT_NAME}
)

install (DIRECTORY
  launch
  config
  DESTINATION share/${PROJECT_NAME}
)

if(BUILD_TESTING)
  find_package(ament_lint_auto REQUIRED)
  # the following line skips the linter which checks for copyrights
  # comment the line when a copyright and licaense is added to all source files
  set(ament_cmake_copyright_FOUND TRUE)
  # the following line skips cpplint (only works in a git repo)
  # comment the line when this package is in a git repo and when
  # a copyright and license is added to all source files
  set(ament_cmake_cpplint_FOUND TRUE)
  ament_lint_auto_find_test_dependencies()
endif()



ament_package()
