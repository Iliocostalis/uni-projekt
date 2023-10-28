#include <OhmCarSimulatorCam.h>
#if DEFINED(USE_OHMCARSIMULATOR)
#include <OhmCarSimulatorConnector.h>
#include <ImageProcessing.h>
#include <filesystem>
#include <iostream>
#include <chrono>
#include <iostream>
#include <thread>

int readInt(uint8_t* data, int offset)
{
    return (int)data[offset] << 24 | (int)data[offset+1] << 16 |(int)data[offset+2] << 8 | (int)data[offset+3];
}

void OhmCarSimulatorCam::cameraLoop()
{
	int index = 0;

    uint8_t header[6];
    uint8_t buffer[64];
    std::vector<uint8_t> imageData;
    imageData.resize(1920*1080*3);

    OhmCarSimulatorConnector* connector = OhmCarSimulatorConnector::getInstance();
	while (cameraRunning)
	{
        if(!connector->isSocketConnected)
            continue;

        int iResult = recv(connector->ConnectSocket, (char*)header, 6, 0);
        int type = header[0];
        int address = header[1];
        int readSize = readInt(header, 2);

        if(type != TYPE_2D_CAM)
        {
            int readCurrent = 0;
            while(readCurrent < readSize)
            {
                int readLength = min(32, readSize - readCurrent);
                iResult = recv(connector->ConnectSocket, (char*)buffer, readLength, 0);
                readCurrent += iResult;
            }
        }
        else
        {
            iResult = recv(connector->ConnectSocket, (char*)buffer, 9, 0);
            int width = readInt(buffer, 0);
            int height = readInt(buffer, 4);
            int colorFormat = buffer[8];

            if(width*height*3 > imageData.size())
            {
                imageData.resize(width*height*3);
            }
            
            int readCurrent = 9;
            while(readCurrent < readSize)
            {
                iResult = recv(connector->ConnectSocket, (char*)imageData.data()+(readCurrent-9), readSize - readCurrent, 0);
                readCurrent += iResult;
            }

            if(colorFormat == COLOR_FORMAT_BW)
            {
                ImageProcessing::process(imageData.data(), 0);
		        //std::this_thread::sleep_for(std::chrono::milliseconds(20));
            }
        }
	}

	return;
}

OhmCarSimulatorCam::OhmCarSimulatorCam() : cameraRunning(false) {}

void OhmCarSimulatorCam::init()
{
	initSuccessful = true;
}

bool OhmCarSimulatorCam::wasInitSuccessful()
{
	return initSuccessful;
}

void OhmCarSimulatorCam::start()
{
	cameraRunning = true;
	camThread = std::thread(std::bind(&OhmCarSimulatorCam::cameraLoop, this));
}

void OhmCarSimulatorCam::stop()
{
	cameraRunning = false;
	if (camThread.joinable())
		camThread.join();
}
#endif