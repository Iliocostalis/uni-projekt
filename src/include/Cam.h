#pragma once
#include <Config.h>
#include <queue>
#include <memory>
#include <list>
#include <functional>
#include <atomic>
#if !DEFINED(PC_MODE)
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
    std::map<libcamera::FrameBuffer*, std::vector<libcamera::Span<uint8_t>>> mapped_buffers;

    std::atomic_bool threadRunning;
    std::atomic_bool cameraRunning;

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
#else
class Cam
{
    pthread_t thread;
    std::vector<std::string> imageNames;

    std::atomic_bool cameraRunning;
    std::vector<std::vector<uint8_t>> images;

    void* loopArg;

    Cam();
    ~Cam();
public:

    Cam(Cam const&)             = delete;
    void operator=(Cam const&)  = delete;

    static Cam* getInstance();   

    void processRequest();
    void init();
    void start();
    void stop(); 
};
#endif