#pragma once
#include <list>
#include <functional>
#include <atomic>
#include <thread>
#include <vector>
#include <string>
#include <Config.h>
#include <ICamera.h>

class DevCam : public ICamera
{
    std::vector<std::string> imageNames;
    std::vector<std::vector<uint8_t>> images;
    std::thread camThread;
    std::atomic_bool cameraRunning;

    DevCam();    
    virtual void cameraLoop();

public:
    friend class CameraCreator;
    std::list<std::function<void(void)>> queue;

    DevCam(const DevCam&) = delete;
    void operator=(const DevCam&) = delete;

    virtual void init();
    virtual bool wasInitSuccessful();
    virtual void start();
    virtual void stop();
};