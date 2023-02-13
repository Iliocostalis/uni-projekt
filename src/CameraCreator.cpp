#include <Config.h>
#include <CameraCreator.h>
#include <PiCam.h>
#include <DevCam.h>

ICamera* CameraCreator::getCamera() {
#if DEFINED(RASPBERRY)
    static PiCam piCam;
    return &piCam;
#else
    static DevCam devCam;
    return &devCam;
#endif
}