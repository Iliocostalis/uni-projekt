#pragma once
#include <Config.h>
#if DEFINED(RASPBERRY)
#include <queue>
#include <memory>
#include <list>
#include <functional>
#include <atomic>
#include <thread>
#include <vector>
#include <string>
#include <map>
#include <condition_variable>
#include <libcamera/libcamera.h>
#include <ICamera.h>
#include <CameraCreator.h>

class PiCam : public ICamera
{
    std::shared_ptr<libcamera::CameraManager> cameraManager;
    std::shared_ptr<libcamera::Camera> camera;
    std::queue<libcamera::Request*> requestQueue;
    std::unique_ptr<libcamera::CameraConfiguration> config;
    std::unique_ptr<libcamera::FrameBufferAllocator> allocator;
    libcamera::ControlList controls;
	std::vector<std::unique_ptr<libcamera::Request>> requests;
    std::map<libcamera::FrameBuffer*, std::vector<libcamera::Span<uint8_t>>> mapped_buffers;

    
    std::thread camThread;
    std::atomic_bool threadRunning;
    std::atomic_bool cameraRunning;

    const int framerate = 60;
    const int imageBufferCount = 6;


    PiCam();
    virtual void cameraLoop();
    
    std::vector<libcamera::Span<uint8_t>> Mmap(libcamera::FrameBuffer *buffer);
    
public:
    friend class CameraCreator;

    std::condition_variable notifyNewImageReady;
    std::list<std::function<void(void)>> queue;

    PiCam(const PiCam&)             = delete;
    void operator=(const PiCam&)  = delete;

    void processRequest(libcamera::Request *request);


    virtual void init();
    virtual bool wasInitSuccessful();
    virtual void start();
    virtual void stop();
};
#endif