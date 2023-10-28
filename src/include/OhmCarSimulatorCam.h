#pragma once
#include <Debugging.h>
#if DEFINED(USE_OHMCARSIMULATOR)
#include <winsock2.h>
#include <list>
#include <functional>
#include <atomic>
#include <thread>
#include <vector>
#include <string>
#include <Config.h>
#include <ICamera.h>

class OhmCarSimulatorCam : public ICamera
{
    std::thread camThread;
    std::atomic_bool cameraRunning;

    OhmCarSimulatorCam();    
    virtual void cameraLoop();

public:
    friend class CameraCreator;
    std::list<std::function<void(void)>> queue;

    OhmCarSimulatorCam(const OhmCarSimulatorCam&) = delete;
    void operator=(const OhmCarSimulatorCam&) = delete;

    virtual void init();
    virtual bool wasInitSuccessful();
    virtual void start();
    virtual void stop();
};
#endif