#include <Controller.h>
#if DEFINED(RASPBERRY)
#include <pigpio.h>
#include <thread>
#include <chrono>
#include <iostream>
#include <functional>


#define MOTOR_PWM_PIN 23
#define MOTOR_DIRECTION_PIN 24

#define AX_PIN 18

//important AX-12 constants
#define AX_ID 1
#define AX_START 255
#define AX_POSITION_LENGTH 5
#define AX_POSITION 30

#define AX_REG_WRITE 4
#define AX_WRITE_DATA 3

uint8_t buffer[32];

void Controller::setDirection(bool out)
{
    if(out)
        gpioWrite(AX_PIN, 1);
    else
        gpioWrite(AX_PIN, 0);

    std::this_thread::sleep_for(std::chrono::microseconds(1));
}

void Controller::loop()
{
    while(loopRunning)
    {
        auto begin = std::chrono::high_resolution_clock::now();

        move();
        applyThrotle();

        // limit to 30 hz
	    auto end = std::chrono::high_resolution_clock::now();
        auto microseconds = std::chrono::duration_cast<std::chrono::microseconds>(end - begin);
        
        int sleep = std::max(0, 33333 - (int)microseconds.count());
		std::this_thread::sleep_for(std::chrono::microseconds(sleep));
    }
	return;
}

void Controller::move()
{
    uint8_t data[] = {0xFF, 0xFF, 0x01, 0x05, 0x03, 0x1E, 0xCD, 0x00, 0x0B};

    int position = (int)(rotation*180.f);
    uint8_t positionH = (uint8_t)((position >> 8) & 0xff);
    uint8_t positionL = (uint8_t)((position) & 0xff);
    
    //AX_REG_WRITE
    uint8_t checksum = (uint8_t)((~(AX_ID + AX_POSITION_LENGTH + AX_WRITE_DATA + AX_POSITION + positionL + positionH))&0xff);
    data[0] = AX_START;
    data[1] = AX_START;
    data[2] = AX_ID;
    data[3] = AX_POSITION_LENGTH;
    data[4] = AX_WRITE_DATA;
    data[5] = AX_POSITION;
    data[6] = positionL;
    data[7] = positionH;
    data[8] = checksum;

    setDirection(true);
    std::this_thread::sleep_for(std::chrono::microseconds(20));

    serWrite(handle, (char*)data, 9);

    std::this_thread::sleep_for(std::chrono::microseconds(2000));
    setDirection(false);

    serRead(handle, (char*)buffer, serDataAvailable(handle));
}

void Controller::applyThrotle()
{
    if(throtle < 0.f)
    {
        pwm = (int)(-throtle*255.0f + 0.5f);
        gpioWrite(MOTOR_DIRECTION_PIN, 0);
    }
    else
    {
        pwm = (int)(throtle*255.0f + 0.5f);
        gpioWrite(MOTOR_DIRECTION_PIN, 1);
    }
    
    gpioPWM(MOTOR_PWM_PIN, pwm);
}



Controller::Controller() : loopRunning(false), rotation(0.f), throtle(0.f) {}

Controller* Controller::getInstance()
{
    static Controller controller;
    return &controller;
}

void Controller::start()
{
    gpioInitialise();
    handle = serOpen("/dev/ttyS0", 57600, 0);
    gpioSetMode(AX_PIN, PI_OUTPUT);

    gpioSetMode(MOTOR_DIRECTION_PIN, PI_OUTPUT);
    //gpioSetPWMfrequency(MOTOR_PWM_PIN, 500); // 500 hz
    gpioPWM(MOTOR_PWM_PIN, 0);
    setThrotle(0.0f);

    if(handle < 0)
    {
        std::cout << "controller starting error! Handle: " << handle << std::endl;
        return;
    }

    loopRunning = true;
    controllerThread = std::thread(std::bind(&Controller::loop, this));
}

void Controller::stop()
{
    // wait for throtle to be zero
    setThrotle(0.0f);
    while(pwm != 0) {std::this_thread::sleep_for(std::chrono::milliseconds(1));}

    loopRunning = false;
    if(controllerThread.joinable())
        controllerThread.join();

    gpioTerminate();
}

void Controller::setThrotle(float value)
{
    throtle = std::min(1.f, std::max(-1.f, value));
}

float Controller::getThrotle()
{
    return throtle;
}

void Controller::setRotation(float value)
{
    rotation = std::min(1.f, std::max(-1.f, value));
}

float Controller::getRotation()
{
    return rotation;
}

#endif