cmake_minimum_required(VERSION 3.16)

project(cam_code)

if(COMPILE_FOR_RASPBERRY)
set(CMAKE_BUILD_TYPE Release)
set(CMAKE_CXX_FLAGS_RELEASE "-O2")
add_compile_definitions(PC_MODE=0)
else()
add_compile_definitions(PC_MODE=1)
endif(COMPILE_FOR_RASPBERRY)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17")

add_executable(cam_code "")
add_subdirectory("src")

if(COMPILE_FOR_RASPBERRY)
target_link_libraries(cam_code libcamera.so libcamera-base.so pthread pigpio X11)
else()
target_link_libraries(cam_code pthread X11)
endif(COMPILE_FOR_RASPBERRY)
unset(COMPILE_FOR_RASPBERRY CACHE)
