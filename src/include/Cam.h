#pragma once
#include <Config.h>
#include <queue>
#include <memory>
#include <list>
#include <functional>
#include <atomic>
#include <thread>
#include <vector>
#include <string>
#if DEFINED(RASPBERRY)
#include <map>
#include <libcamera/libcamera.h>
#include <condition_variable>
#endif

class Cam
{
#if DEFINED(RASPBERRY)
    std::shared_ptr<libcamera::CameraManager> cameraManager;
    std::shared_ptr<libcamera::Camera> camera;
    std::queue<libcamera::Request*> requestQueue;
    std::unique_ptr<libcamera::CameraConfiguration> config;
    std::unique_ptr<libcamera::FrameBufferAllocator> allocator;
    libcamera::ControlList controls;
	std::vector<std::unique_ptr<libcamera::Request>> requests;
    std::map<libcamera::FrameBuffer*, std::vector<libcamera::Span<uint8_t>>> mapped_buffers;

    std::condition_variable newImage;
#else
    std::vector<std::string> imageNames;
    std::vector<std::vector<uint8_t>> images;

#endif
    std::thread camThread;
    std::atomic_bool threadRunning;
    std::atomic_bool cameraRunning;


    Cam();
    void cameraLoop();
#if DEFINED(RASPBERRY)
    std::vector<libcamera::Span<uint8_t>> Mmap(libcamera::FrameBuffer *buffer);
#endif
public:
    std::list<std::function<void(void)>> queue;

    Cam(Cam const&)             = delete;
    void operator=(Cam const&)  = delete;

    static Cam* getInstance();   

#if DEFINED(RASPBERRY)
    void processRequest(libcamera::Request *request);
#endif

    void init();
    void start();
    void stop(); 
};