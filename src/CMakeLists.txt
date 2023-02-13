cmake_minimum_required(VERSION 3.16)

IF(DEFINED ENV{LIBCAMERA})
link_directories("$ENV{LIBCAMERA}/src/libcamera")
link_directories("$ENV{LIBCAMERA}/src/libcamera/base")

target_include_directories(cam_code PUBLIC "$ENV{LIBCAMERA}/build/include/")
target_include_directories(cam_code PUBLIC "$ENV{LIBCAMERA}/include/")

ENDIF()

target_include_directories(cam_code PUBLIC ${CMAKE_CURRENT_LIST_DIR}/include)
target_sources(cam_code PRIVATE Main.cpp PiCam.cpp ImageProcessing.cpp Controller.cpp Utils.cpp CameraCreator.cpp DevCam.cpp)
