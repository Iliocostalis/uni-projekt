#include <iostream>
#include <iomanip>
#include <thread>

#include <stdio.h>
#include <stdlib.h>

#include <Config.h>
#include <Cam.h>
#include <ImageProcessing.h>
#include <Preview.h>

int main()
{
	ImageProcessing::init();

#if DEFINED(SHOW_PREVIEW)
    Preview::getInstance()->open();
#endif


	Cam cam;
	cam.init();
	cam.start();

	auto start = std::chrono::high_resolution_clock::now();
	auto last = std::chrono::high_resolution_clock::now();
    while(true)
    {
		auto now = std::chrono::high_resolution_clock::now();
        auto milliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(now - last);
        auto runningTime = std::chrono::duration_cast<std::chrono::seconds>(now - start);

		if(runningTime.count() >= 10)
			break;

		if(milliseconds.count() > 50)
		{
			last = now;

#if DEFINED(SHOW_PREVIEW)
			std::cout << "refresh window" << std::endl;
			std::cout << "image index: " << ImageProcessing::currentImageIndex << std::endl;
			Preview::getInstance()->refresh();
#endif
		}
    }

#if DEFINED(SHOW_PREVIEW)
    Preview::getInstance()->close();
#endif

	cam.stop();
	
	return 0;
}