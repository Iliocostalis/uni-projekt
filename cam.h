#pragma once
#include <queue>
#include <memory>
#include <libcamera/libcamera.h>

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