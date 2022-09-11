#include <iostream>
#include <iomanip>
#include <pthread.h>
#include <chrono>

#include <stdio.h>
#include <stdlib.h>

#include <Config.h>
#include <Cam.h>
#include <ImageProcessing.h>
#include <Preview.h>
#include <Controller.h>

std::atomic_bool isLoopRunning(false);
pthread_t loopThread;

void* loop(void* arg)
{
	auto start = std::chrono::high_resolution_clock::now();
	auto last = std::chrono::high_resolution_clock::now();
    while(isLoopRunning)
    {
		auto now = std::chrono::high_resolution_clock::now();
        auto milliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(now - last);

		// limit to 20 fps
		if(milliseconds.count() > 50)
		{
			last = now;

#if DEFINED(SHOW_PREVIEW)
#if DEFINED(PREVIEW_LOG)
			std::cout << "refresh window" << std::endl;
			std::cout << "image index: " << ImageProcessing::currentImageIndex << std::endl;
#endif
			Preview::getInstance()->refresh();
#endif
		}
    }
    return (void*)nullptr;
}

int main()
{
	//Controller::getInstance()->start();
	//Controller::getInstance()->stop();
	//return 0;

	ImageProcessing::init();

#if DEFINED(SHOW_PREVIEW)
    Preview::getInstance()->open();
#endif

	Cam::getInstance()->init();
	Cam::getInstance()->start();

	isLoopRunning = true;
    int ret = pthread_create(&loopThread, NULL, &loop, NULL);

    while(true)
    {
		std::string line;
		std::getline( std::cin, line );

		std::cout << line << std::endl;
		if(line == "stop")
		{
			isLoopRunning = false;
			break;
		}
		else if(line == "save")
		{
			ImageProcessing::saveImage();
		}
		else if(line == "saveVideo")
		{
			ImageProcessing::saveVideo();
		}
		else if(line == "stopVideo")
		{
			ImageProcessing::stopVideo();
		}
    }

	void* returnValue;
	pthread_join(loopThread, &returnValue);

#if DEFINED(SHOW_PREVIEW)
    Preview::getInstance()->close();
#endif

	Cam::getInstance()->stop();
	
	return 0;
}
