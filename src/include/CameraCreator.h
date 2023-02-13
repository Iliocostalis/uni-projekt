#pragma once
#include <ICamera.h>

class CameraCreator {
    public:
        static ICamera* getCamera();
};