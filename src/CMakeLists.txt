cmake_minimum_required(VERSION 3.16)

link_directories("${CMAKE_CURRENT_LIST_DIR}/libcamera/src/libcamera")
link_directories("${CMAKE_CURRENT_LIST_DIR}/libcamera/src/libcamera/base")

target_include_directories(cam_code PUBLIC ${CMAKE_CURRENT_LIST_DIR}/include)
target_include_directories(cam_code PUBLIC "${CMAKE_CURRENT_LIST_DIR}/libcamera/build/include/")
target_include_directories(cam_code PUBLIC "${CMAKE_CURRENT_LIST_DIR}/libcamera/include/")

#file(GLOB srcFiles CONFIGURE_DEPENDS "*.cpp")
#target_sources(cam_code PRIVATE ${srcFiles})
target_sources(cam_code PRIVATE Main.cpp Cam.cpp ImageProcessing.cpp Controller.cpp Utils.cpp)
