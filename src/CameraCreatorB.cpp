#include <Utils.h>
//#include <PiCam.h>
//#include <DevCam.h>
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