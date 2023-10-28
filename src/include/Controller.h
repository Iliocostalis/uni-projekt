#pragma once
#include <Config.h>
#include <atomic>
#include <thread>
#include <chrono>

class Controller
{
    int handle;
    std::atomic_bool isLoopRunning;
    std::thread controllerThread;
    float rotation;
    float throttle;
    int pwm;
    float percentageDarkPixelsInStartStopLine;
    bool isMotorOn;
    bool isMotorSwitchingState;
    
    std::chrono::system_clock::time_point timeMotorWillSwitchState;
    
    Controller();
    void loop();
    void move();
    void applyThrottle();
    void setDirectionOut();
    void setDirectionIn();
    void updateStartStop();
    void startMotor();
    void stopMotor();

public:
    Controller(const Controller&)      = delete;
    void operator=(const Controller&)  = delete;

    static Controller* getInstance();

    void start();
    void stop();

    void setThrottle(float value);
    float getThrottle();
    void setRotation(float value);
    float getRotation();

    void updatePercentageDarkPixelsInStartStopLine(float value);
};