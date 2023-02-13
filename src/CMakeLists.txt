cmake_minimum_required(VERSION 3.16)

IF(DEFINED ENV{LIBCAMERA})
link_directories("{LIBCAMERA}/src/libcamera")
link_directories("{LIBCAMERA}/src/libcamera/base")

target_include_directories(cam_code PUBLIC "${LIBCAMERA}/build/include/")
target_include_directories(cam_code PUBLIC "${LIBCAMERA}/include/")

ENDIF()

target_include_directories(cam_code PUBLIC ${CMAKE_CURRENT_LIST_DIR}/include)
#file(GLOB srcFiles CONFIGURE_DEPENDS "*.cpp")
#target_sources(cam_code PRIVATE ${srcFiles})
target_sources(cam_code PRIVATE Main.cpp Cam.cpp ImageProcessing.cpp Controller.cpp Utils.cpp CameraCreator.cpp)
