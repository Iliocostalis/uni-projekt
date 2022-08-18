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

bool isRunning = true;
pthread_t loopThread;

void* loop(void* arg)
{
	auto start = std::chrono::high_resolution_clock::now();
	auto last = std::chrono::high_resolution_clock::now();
    while(true)
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
}

int main()
{
	ImageProcessing::init();

#if DEFINED(SHOW_PREVIEW)
    Preview::getInstance()->open();
#endif

	Cam::getInstance()->init();
	Cam::getInstance()->start();

    int ret = pthread_create(&loopThread, NULL, &loop, NULL);

    while(true)
    {
		std::string line;
		std::getline( std::cin, line );

		if(line == "stop")
		{
			isRunning = false;
			break;
		}
		else
			std::cout << line << std::endl;
    }

	void* returnValue;
	pthread_join(loopThread, &returnValue);

#if DEFINED(SHOW_PREVIEW)
    Preview::getInstance()->close();
#endif

	Cam::getInstance()->stop();
	
	return 0;
}