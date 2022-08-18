cmake_minimum_required(VERSION 3.16)

project(cam_code)
set(CMAKE_BUILD_TYPE Release)
set(CMAKE_CXX_FLAGS_RELEASE "-O2")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17")

link_directories("${CMAKE_CURRENT_SOURCE_DIR}/libcamera/src/libcamera")
link_directories("${CMAKE_CURRENT_SOURCE_DIR}/libcamera/src/libcamera/base")

include_directories("${CMAKE_CURRENT_SOURCE_DIR}/libcamera/build/include/")
include_directories("${CMAKE_CURRENT_SOURCE_DIR}/libcamera/include/")
include_directories("${CMAKE_CURRENT_SOURCE_DIR}")

add_executable(cam_code Main.cpp Cam.cpp ImageProcessing.cpp Preview.cpp)
target_link_libraries(cam_code libcamera.so libcamera-base.so pthread pigpio X11)