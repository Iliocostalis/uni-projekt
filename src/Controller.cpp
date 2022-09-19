#include <Controller.h>
#if !DEFINED(PC_MODE)
#include <pigpio.h>
#include <thread>
#include <chrono>
#include <iostream>


#define MOTOR_PWM_PIN 23
#define MOTOR_DIRECTION_PIN 24



int handle;
#define AX_PIN 18

//important AX-12 constants
#define AX_ID 1
#define AX_START 255
#define AX_POSITION_LENGTH 5
#define AX_POSITION 30

#define AX_SYNC_WRITE 0x83
#define AX_RESET 6
#define AX_ACTION 5
#define AX_REG_WRITE 4
#define AX_WRITE_DATA 3
#define AX_READ_DATA 2			
#define AX_PING 1
#define BROADCASTID 0xFE

uint8_t buffer[32];

void setDirection(bool out)
{
    if(out)
        gpioWrite(AX_PIN, 1);
    else
        gpioWrite(AX_PIN, 0);

    std::this_thread::sleep_for(std::chrono::microseconds(1));
}

void getStatus(int index)
{
    setDirection(false);

    int count = 0;
    while(true)
    {
        if(serDataAvailable(handle))
            break;

        if(count >= 30000)
            break;

        count++;
        std::this_thread::sleep_for(std::chrono::microseconds(10));
    }

    int b1 = serReadByte(handle);

    if(b1 != 0xff)
    {
        std::cout << "no response" << std::endl;
        return;
    }



    int b2 = serReadByte(handle);
    int origin = serReadByte(handle);
    int length = serReadByte(handle) - 1;
    int error = serReadByte(handle);

    for(int i = 0; i < length; i++)
    {
        serReadByte(handle);
    }

    if(error != 0) // error
        std::cout << "error" << std::endl;
}

void Controller::move(float value)
{
    uint8_t data[] = {0xFF, 0xFF, 0x01, 0x05, 0x03, 0x1E, 0xCD, 0x00, 0x0B};

    int position = (int)(value*180.f);
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

void Controller::applyThrotle(float value)
{
    int pwm = 0;
    if(value < 0.f)
    {
        pwm = (int)(-value*255.0f + 0.5f);
        gpioWrite(MOTOR_DIRECTION_PIN, 0);
    }
    else
    {
        pwm = (int)(value*255.0f + 0.5f);
        gpioWrite(MOTOR_DIRECTION_PIN, 1);
    }
    
    gpioPWM(MOTOR_PWM_PIN, pwm);
}

void ping(int index)
{
    setDirection(true);
	uint8_t checksum = 255 - ((index + AX_PING + 2)%256); // calculate the checksum	
    buffer[0] = 0xff;
    buffer[1] = 0xff;
    buffer[2] = (uint8_t) index;
    buffer[3] = 0x02;
    buffer[4] = (uint8_t) AX_PING;
    buffer[5] = checksum;
	//outData = chr(0xFF)+chr(0xFF)+chr(index)+chr(0x02)+chr(AX_PING)+chr(checksum);	// build a string with the first part
	//port.write(outData)	// write it out of the serial port	
    serWrite(handle, (char*)buffer, 6);
	getStatus(index);
}




Controller::Controller() : loopRunning(false), rotation(0.f), throtle(0.f) {}

Controller* Controller::getInstance()
{
    static Controller controller;
    return &controller;
}

void* controllerLoop(void* arg)
{
    Controller* controller = Controller::getInstance();
    float lastRot = controller->getRotation() + 1.f; //force update
    float lastThrotle = controller->getThrotle() + 1.f; //force update

    while(loopRunning)
    {
        auto begin = std::chrono::high_resolution_clock::now();

        float rot = controller->getRotation();
        float throtle = controller->getThrotle();

        if(lastRot != rot)
        {
            lastRot = rot;
            controller->move(rot);
        }
        if(lastThrotle != throtle)
        {
            lastThrotle = throtle;
            controller->applyThrotle(throtle);
        }

        // limit to 30 hz
	    auto end = std::chrono::high_resolution_clock::now();
        auto microseconds = std::chrono::duration_cast<std::chrono::microseconds>(end - begin);
        
        int sleep = std::max(0, 33333 - (int)microseconds.count());
		std::this_thread::sleep_for(std::chrono::microseconds(sleep));
    }
	return (void*)nullptr;
}

void Controller::start()
{
    gpioInitialise();
    handle = serOpen("/dev/ttyS0", 57600, 0);
    gpioSetMode(AX_PIN, PI_OUTPUT);

    gpioSetMode(MOTOR_DIRECTION_PIN, PI_OUTPUT);
    //gpioSetPWMfrequency(MOTOR_PWM_PIN, 500); // 500 hz
    gpioPWM(MOTOR_PWM_PIN, 0);

    if(handle < 0)
    {
        std::cout << "controller starting error! Handle: " << handle << std::endl;
        return;
    }

    loopRunning = true;
    int threadRet;
    threadRet = pthread_create(&loopThread, NULL, &controllerLoop, (void*) nullptr);

    //for(int i = 0; i <= 255; i++)
    //{
    //    ping(i);
    //}
    
    /*setDirection(true);
    uint8_t testBuffer[] = {0xFF, 0xFF, 0x01, 0x04, 0x03, 0x04, 0x01, 0xF2};
    uint8_t testRot1[] = {0xFF, 0xFF, 0x01, 0x05, 0x03, 0x1E, 0x32, 0x03, 0xA3};
    uint8_t testRot2[] = {0xFF, 0xFF, 0x01, 0x05, 0x03, 0x1E, 0xCD, 0x00, 0x0B};
    uint8_t buff[32];*/
    /*while(true)
    {
        setDirection(true);
        std::this_thread::sleep_for(std::chrono::microseconds(20));
        serWrite(handle, (char*)testBuffer, 8);
        
        std::this_thread::sleep_for(std::chrono::microseconds(2000));
    
        setDirection(false);
        
        std::cout << (int)serDataAvailable(handle) << std::endl;
        std::this_thread::sleep_for(std::chrono::microseconds(250));
        std::cout << (int)serDataAvailable(handle) << std::endl;
        std::this_thread::sleep_for(std::chrono::microseconds(250));
        std::cout << (int)serDataAvailable(handle) << std::endl;
        std::this_thread::sleep_for(std::chrono::microseconds(250));
        std::cout << (int)serDataAvailable(handle) << std::endl;
        std::this_thread::sleep_for(std::chrono::microseconds(250000));
    }*/
    
    /*while(true)
    {
        setDirection(true);
        std::this_thread::sleep_for(std::chrono::microseconds(20));
        serWrite(handle, (char*)testRot1, 9);
        
        std::this_thread::sleep_for(std::chrono::microseconds(2000));
    
        setDirection(false);
        
        std::cout << (int)serDataAvailable(handle) << std::endl;
        std::this_thread::sleep_for(std::chrono::microseconds(250));
        std::cout << (int)serDataAvailable(handle) << std::endl;
        std::this_thread::sleep_for(std::chrono::microseconds(250));
        std::cout << (int)serDataAvailable(handle) << std::endl;
        std::this_thread::sleep_for(std::chrono::microseconds(250));
        std::cout << (int)serDataAvailable(handle) << std::endl;
        std::this_thread::sleep_for(std::chrono::microseconds(1250000));
        
        std::cout << (int)serDataAvailable(handle) << std::endl;
        serRead(handle, (char*)buff, serDataAvailable(handle));
        
        
        setDirection(true);
        std::this_thread::sleep_for(std::chrono::microseconds(20));
        serWrite(handle, (char*)testRot2, 9);
        
        std::this_thread::sleep_for(std::chrono::microseconds(2000));
    
        setDirection(false);
        
        std::cout << (int)serDataAvailable(handle) << std::endl;
        std::this_thread::sleep_for(std::chrono::microseconds(250));
        std::cout << (int)serDataAvailable(handle) << std::endl;
        std::this_thread::sleep_for(std::chrono::microseconds(250));
        std::cout << (int)serDataAvailable(handle) << std::endl;
        std::this_thread::sleep_for(std::chrono::microseconds(250));
        std::cout << (int)serDataAvailable(handle) << std::endl;
        std::this_thread::sleep_for(std::chrono::microseconds(1250000));
        
        std::cout << (int)serDataAvailable(handle) << std::endl;
        serRead(handle, (char*)buff, serDataAvailable(handle));
    }*/
}

void Controller::stop()
{
    loopRunning = false;
	void* returnVal;
	pthread_join(loopThread, &returnVal);

    gpioTerminate();
}

void Controller::setThrotle(float value)
{
    throtle = std::min(1.f, std::max(-1.f, value));
}

void Controller::setRotation(float value)
{
    rotation = std::min(1.f, std::max(-1.f, value));
}

#endif