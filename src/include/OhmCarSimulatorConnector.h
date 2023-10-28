#pragma once
#include <Debugging.h>
#if DEFINED(USE_OHMCARSIMULATOR)
#define NOMINMAX
#include <winsock2.h>
#include <stdint.h>

#define DEFAULT_PORT "26134"
#define TYPE_CAR 0
#define TYPE_2D_CAM 1
#define TYPE_3D_CAM 2
#define TYPE_DISTANCE_SENSOR 3
#define TYPE_LIDAR 4

#define COLOR_FORMAT_BW 0
#define COLOR_FORMAT_RGB 1
#define COLOR_FORMAT_BGR 2
#define COLOR_FORMAT_D8 3
#define COLOR_FORMAT_D16 4

class OhmCarSimulatorConnector
{
public:
    volatile int client_fd;
    volatile bool isSocketConnected;
    volatile SOCKET ConnectSocket = INVALID_SOCKET;
    
private:
    OhmCarSimulatorConnector();   

public:
    OhmCarSimulatorConnector(const OhmCarSimulatorConnector&)      = delete;
    void operator=(const OhmCarSimulatorConnector&)  = delete;

    static OhmCarSimulatorConnector* getInstance();
    static void floatToBytes(float value, uint8_t* buffer, int offset);
    static int readInt(uint8_t* data, int offset);
    static void intToBytes(int value, uint8_t* data, int offset);

    void startConnection();
    void closeConnection();
    void sendData(uint8_t* data, int length);
};
#endif