#include <Controller.h>
#if !DEFINED(PC_MODE)
#include <pigpio.h>
#include <thread>
#include <chrono>
#include <iostream>

struct Car
{
    float throtle;
    float steering;
};

int handle;
#define PIN 18

//important AX-12 constants
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
        gpioWrite(PIN, 1);
    else
        gpioWrite(PIN, 0);

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




Controller::Controller(){}

Controller* Controller::getInstance()
{
    static Controller controller;
    return &controller;
}

void Controller::start()
{
    gpioInitialise();
    handle = serOpen("/dev/ttyS0", 57600, 0);
    gpioSetMode(PIN, PI_OUTPUT);

    if(handle < 0)
    {
        std::cout << handle << std::endl;
        return;
    }

    //for(int i = 0; i <= 255; i++)
    //{
    //    ping(i);
    //}
    
    setDirection(true);
    uint8_t testBuffer[] = {0xFF, 0xFF, 0x01, 0x04, 0x03, 0x04, 0x01, 0xF2};
    uint8_t testRot1[] = {0xFF, 0xFF, 0x01, 0x05, 0x03, 0x1E, 0x32, 0x03, 0xA3};
    uint8_t testRot2[] = {0xFF, 0xFF, 0x01, 0x05, 0x03, 0x1E, 0xCD, 0x00, 0x0B};
    uint8_t buff[32];
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
    
    while(true)
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
    }
}

void Controller::stop()
{

    gpioTerminate();
}
#endif