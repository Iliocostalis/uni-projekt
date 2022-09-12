#include <iostream>
#include <Config.h>
#include <Cam.h>
#include <ImageProcessing.h>
#include <Preview.h>
#include <Controller.h>
#include <Utils.h>
#include <mutex>

std::mutex m;
//extern std::atomic_bool stop(false);
std::atomic_bool inputThreadRunning(false);

void* consoleInputLoop(void*)
{
	//std::lock_guard lk(m);
	while(true)
    {
		std::string line;
		std::getline( std::cin, line );

		if(line == "stop")
		{
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
	inputThreadRunning = false;
	stop.notify_all();
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
	
	inputThreadRunning = true;
	pthread_t inputThread; 
    int ret = pthread_create(&inputThread, NULL, &consoleInputLoop, NULL);
	
    std::unique_lock lk(m);
	stop.wait(lk);
	lk.unlock();

	if(inputThreadRunning)
		pthread_cancel(inputThread);


	// close all
#if DEFINED(SHOW_PREVIEW)
    Preview::getInstance()->close();
#endif

	Cam::getInstance()->stop();
	
	return 0;
}
