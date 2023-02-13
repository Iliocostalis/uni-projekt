#pragma once
#include <Config.h>
#if DEFINED(RASPBERRY)
#include <atomic>
#include <thread>

class Controller
{
    int handle;
    std::atomic_bool isLoopRunning;
    std::thread controllerThread;
    float rotation;
    float throtle;
    int pwm;
    
    Controller();
    void loop();
    void move();
    void applyThrotle();
    void setDirectionOut();
    void setDirectionIn();

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
};
#endif