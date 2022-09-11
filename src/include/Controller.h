#pragma once
#include <Config.h>
#if !DEFINED(PC_MODE)

class Controller
{
    Controller();
    
public:
    Controller(Controller const&)      = delete;
    void operator=(Controller const&)  = delete;

    static Controller* getInstance();

    void start();
    void stop();
};
#endif