#include <Utils.h>
//#include <PiCam.h>
//#include <DevCam.h>

#include <Config.h>
#include <queue>
#include <memory>
#include <list>
#include <functional>
#include <atomic>
#include <thread>
#include <vector>
#include <string>
#include <map>
#include <libcamera/libcamera.h>
#include <condition_variable>
#include <ICamera.h>

#include <CameraCreatorB.h>

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