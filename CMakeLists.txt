cmake_minimum_required(VERSION 3.8)
project(intra_demo_1pub_2sub)

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

find_package(ament_cmake REQUIRED)
find_package(rclcpp REQUIRED)
find_package(std_msgs REQUIRED)

add_executable(cyclic_pipeline src/pipeline.cpp)
ament_target_dependencies(cyclic_pipeline rclcpp std_msgs)

install(TARGETS
  cyclic_pipeline
  DESTINATION lib/${PROJECT_NAME})

ament_package()
