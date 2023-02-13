#include <Config.h>
//#include <PiCam.h>
//#include <DevCam.h>

#include <vector>
#include <stdint.h>
#include <iostream>
#include <thread>
#include <atomic>
#include <functional>
#include <chrono>
#include <X11/Xlib.h>

#include <libcamera/libcamera.h>


#include <CameraCreatorB.h>

ICamera* CameraCreatorB::getCamera() {
#if DEFINED(RASPBERRY)
    //static PiCam piCam;
    //return &piCam;
    return nullptr;
#else
    //static DevCam devCam;
    //return &devCam;
    return nullptr;
#endif
}