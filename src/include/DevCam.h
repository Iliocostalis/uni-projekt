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

    DevCam(DevCam const &) = delete;
    void operator=(DevCam const &) = delete;

    static DevCam *getInstance();

    virtual void init();
    virtual bool wasInitSuccessful();
    virtual void start();
    virtual void stop();
};