#pragma once
#include <Config.h>
#if !DEFINED(PC_MODE) || 1
#include <atomic>
#include <pthread.h>

class Controller
{
    std::atomic_bool loopRunning;
    pthread_t loopThread;
    float rotation;
    float throtle;
    
    Controller();
    void move(float value);
    void applyThrotle(float value);

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