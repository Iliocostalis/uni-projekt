#include <Config.h>
#include <CameraCreator.h>
#include <PiCam.h>
#include <DevCam.h>
#include <OhmCarSimulatorCam.h>

ICamera* CameraCreator::getCamera() {
#if DEFINED(RASPBERRY)
    static PiCam piCam;
    return &piCam;
#elif DEFINED(USE_OHMCARSIMULATOR)
    static OhmCarSimulatorCam ohmCarSimulatorCam;
    return &ohmCarSimulatorCam;
#else
    static DevCam devCam;
    return &devCam;
#endif
}