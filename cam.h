#pragma once
#include <queue>
#include <memory>
//#include <libcamera/libcamera.h>


#include <sys/mman.h>

#include <condition_variable>
#include <iostream>
#include <mutex>
#include <set>
#include <string>
#include <thread>
#include <variant>

#include <libcamera/base/span.h>
#include <libcamera/camera.h>
#include <libcamera/camera_manager.h>
#include <libcamera/control_ids.h>
#include <libcamera/controls.h>
#include <libcamera/formats.h>
#include <libcamera/framebuffer_allocator.h>
#include <libcamera/property_ids.h>

class Cam
{
    std::shared_ptr<libcamera::Camera> camera;
    std::queue<libcamera::Request*> requestQueue;
    std::unique_ptr<libcamera::CameraConfiguration> config;

    void requestComplete(libcamera::Request* request);

public:
    void init();
    void start();
    void stop();    
};