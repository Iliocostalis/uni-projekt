#include <Utils.h>
//#include <PiCam.h>
//#include <DevCam.h>
#include <CameraCreator.h>

ICamera* CameraCreator::getCamera() {
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