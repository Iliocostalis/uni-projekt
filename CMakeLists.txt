cmake_minimum_required(VERSION 3.16)

project(cam_code)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17")

link_directories("${CMAKE_CURRENT_SOURCE_DIR}/libcamera/src/libcamera")

include_directories("${CMAKE_CURRENT_SOURCE_DIR}/libcamera/include/")
include_directories("${CMAKE_CURRENT_SOURCE_DIR}")

add_executable(cam_code main.cpp cam.cpp)

target_link_libraries(cam_code libcamera.so libcamera-base.so)
