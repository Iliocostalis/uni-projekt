cmake_minimum_required(VERSION 3.16)

project(cam_code)

message("CMAKE_SYSTEM_NAME: ${CMAKE_SYSTEM_NAME}")
message("BUILD_TYPE: ${CMAKE_BUILD_TYPE}")

if(${COMPILE_FOR_RASPBERRY})

    message("BUILD_FOR_RASPBERRY")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17")
    set(CMAKE_BUILD_TYPE Release)
    set(CMAKE_CXX_FLAGS_RELEASE "-O2")
    add_executable(cam_code "")
    add_subdirectory("src")
    target_compile_definitions(cam_code PRIVATE RASPBERRY=1)
    target_compile_definitions(cam_code PRIVATE WINDOWS=0)
    target_compile_definitions(cam_code PRIVATE LINUX=0)
    target_link_libraries(cam_code libcamera.so libcamera-base.so pthread pigpio X11)

elseif(${CMAKE_SYSTEM_NAME} MATCHES "Windows")

    message("BUILD_FOR_WINDOWS")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std:c++17")
    add_executable(cam_code "")
    add_subdirectory("src")
    target_compile_definitions(cam_code PRIVATE RASPBERRY=0)
    target_compile_definitions(cam_code PRIVATE WINDOWS=1)
    target_compile_definitions(cam_code PRIVATE LINUX=0)
    target_compile_definitions(cam_code PRIVATE USE_OHMCARSIMULATOR=1)

else()

    message("BUILD_FOR_LINUX")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std:c++17")
    add_executable(cam_code "")
    add_subdirectory("src")
    target_compile_definitions(cam_code PRIVATE RASPBERRY=0)
    target_compile_definitions(cam_code PRIVATE WINDOWS=0)
    target_compile_definitions(cam_code PRIVATE LINUX=1)
    target_link_libraries(cam_code pthread X11)

endif()
unset(COMPILE_FOR_RASPBERRY CACHE)