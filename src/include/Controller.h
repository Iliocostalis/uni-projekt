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
    float throtle;
    int pwm;
    float percentageDarkPixelsInStartStopLine;
    bool isMotorOn;
    bool isMotorSwitchingState;
    std::chrono::steady_clock::time_point timeMotorWillSwitchState;
    
    Controller();
    void loop();
    void move();
    void applyThrotle();
    void setDirectionOut();
    void setDirectionIn();
    void updateStartStop();
    void startMotor();
    void stopMotor();

public:
    Controller(Controller const&)      = delete;
    void operator=(Controller const&)  = delete;

    static Controller* getInstance();

    void start();
    void stop();

    // -1(backwards) to 1(forward) -> 0.5 = 50% throtle forward
    void setThrotle(float value);
    float getThrotle();
    // -1(left) to 1(right)
    void setRotation(float value);
    float getRotation();

    void updatePercentageDarkPixelsInStartStopLine(float value);
};