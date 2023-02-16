#include <Controller.h>
#if DEFINED(RASPBERRY)
#include <pigpio.h>
#endif
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


#if !DEFINED(RASPBERRY)
#define PI_OUTPUT 0
void gpioWrite(int, int){}
void serWrite(int, char*, int){}
void gpioPWM(int, int){}
void gpioInitialise(){}
void gpioTerminate(){}
int serOpen(const char*, int, int){return 1;}
void gpioSetMode(int, int){}
#endif

void preciseSleep(int microseconds)
{
    auto start = std::chrono::high_resolution_clock::now();
    bool waiting = true;
    while(waiting)
    {
	    auto now = std::chrono::high_resolution_clock::now();
        auto microsec = std::chrono::duration_cast<std::chrono::microseconds>(now - start);

        if(microsec.count() >= microseconds)
            waiting = false;
    }
}

void Controller::setDirectionOut()
{
    gpioWrite(AX_PIN, 1);
    std::this_thread::sleep_for(std::chrono::microseconds(1));
}

void Controller::setDirectionIn()
{
    gpioWrite(AX_PIN, 0);
    std::this_thread::sleep_for(std::chrono::microseconds(1));
}

void Controller::updateStartStop()
{
    const float threshold = 0.025f;
    const int detectionDelayInSeconds = 5;
    const int motorOffDelayInSeconds = 5;

    std::cout << "Start/Stop line percentage: " << percentageDarkPixelsInStartStopLine << std::endl;
    
    auto now = std::chrono::system_clock::now();

    if(isMotorSwitchingState && timeMotorWillSwitchState <= now)
    {
        isMotorSwitchingState = false;
        if(isMotorOn)
            stopMotor();
        else
            startMotor();
    }

    if(percentageDarkPixelsInStartStopLine < threshold)
        return;

    if(timeMotorWillSwitchState + std::chrono::seconds(5) > now)
        return;

    std::cout << "Start stop detected" << std::endl;

    if(!isMotorSwitchingState)
    {
        isMotorSwitchingState = true;
        timeMotorWillSwitchState = now + std::chrono::seconds(5);
    }
}

void Controller::startMotor()
{
    throtle = 1.0f;
    isMotorOn = true;
}

void Controller::stopMotor()
{
    throtle = 0;
    isMotorOn = false;
}

void Controller::loop()
{
    while(isLoopRunning)
    {
        auto begin = std::chrono::high_resolution_clock::now();

        updateStartStop();
        move();
        applyThrotle();

        // limit to 30 hz
	    auto end = std::chrono::high_resolution_clock::now();
        auto microseconds = std::chrono::duration_cast<std::chrono::microseconds>(end - begin);
        
        int sleep = std::max(0, 33333 - (int)microseconds.count());
        preciseSleep(sleep);
		//std::this_thread::sleep_for(std::chrono::microseconds(sleep));
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

    setDirectionOut();
    //std::this_thread::sleep_for(std::chrono::microseconds(20));
    preciseSleep(20);

    serWrite(handle, (char*)data, 9);

    //std::this_thread::sleep_for(std::chrono::microseconds(2000));
    preciseSleep(2000);
    //setDirectionIn();

    //serRead(handle, (char*)buffer, serDataAvailable(handle));
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

Controller::Controller() : isLoopRunning(false), isMotorOn(false), isMotorSwitchingState(false), rotation(0.f), throtle(0.f) {}

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
    stopMotor();

    if(handle < 0)
    {
        std::cout << "controller starting error! Handle: " << handle << std::endl;
        return;
    }

    isLoopRunning = true;
    controllerThread = std::thread(std::bind(&Controller::loop, this));
}

void Controller::stop()
{
    // wait for throtle to be zero
    stopMotor();
    while(pwm != 0) {std::this_thread::sleep_for(std::chrono::milliseconds(1));}

    isLoopRunning = false;
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

void Controller::updatePercentageDarkPixelsInStartStopLine(float value)
{
    percentageDarkPixelsInStartStopLine = value;
}