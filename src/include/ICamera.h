#pragma once

class ICamera {
    protected:
        bool initSuccessful = false;
        virtual void cameraLoop() = 0;
    public:
        virtual void init() = 0;
        virtual bool wasInitSuccessful() = 0;
        virtual void start() = 0;
        virtual void stop() = 0;
};