#pragma once
#include <queue>
#include <memory>
#include <list>
#include <functional>
#include <libcamera/libcamera.h>

class Cam
{
    std::shared_ptr<libcamera::CameraManager> cameraManager;
    std::shared_ptr<libcamera::Camera> camera;
    std::queue<libcamera::Request*> requestQueue;
    std::unique_ptr<libcamera::CameraConfiguration> config;
    std::unique_ptr<libcamera::FrameBufferAllocator> allocator;
    pthread_t thread;
    libcamera::ControlList controls;
	std::vector<std::unique_ptr<libcamera::Request>> requests;

    Cam();
public:
    std::list<std::function<void(void)>> queue;


    Cam(Cam const&)             = delete;
    void operator=(Cam const&)  = delete;

    static Cam* getInstance();   

    void processRequest(libcamera::Request *request);
    void init();
    void start();
    void stop(); 
};