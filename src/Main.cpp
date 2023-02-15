#include <Config.h>
#include <iostream>
#include <CameraCreator.h>
#include <ImageProcessing.h>
#include <Controller.h>
#include <Utils.h>
#include <mutex>
#include <string>

#define SIMPLE_WINDOW_IMPLEMENTATION
#if DEFINED(WINDOWS)
#define SIMPLE_WINDOW_WINDOWS
#endif
#include <SimpleWindow.h>

void parseArgs(const std::string& arg)
{
	if(arg == "-preview")
		isPreviewVisible = true;
	else if(arg == "-lines")
		areLinesVisible = true;
	else if(arg == "-record")
		ImageProcessing::saveVideo();
}

int main(int argc, char *argv[])
{
#if DEFINED(DEBUG)
	std::cout << "Debug build" << std::endl;
#else 
	std::cout << "Release build" << std::endl;
#endif

	for(int i = 1; i < argc; ++i)
		parseArgs(std::string(argv[i]));

	areLinesVisible = areLinesVisible && isPreviewVisible;

	ICamera* cam = CameraCreator::getCamera();

	cam->init();
	if(!cam->wasInitSuccessful())
	{
		std::cout << "Camera init error" << std::endl;
		return 0;
	}

	Controller::getInstance()->start();

	if(isPreviewVisible)
		window = new SimpleWindow(IMAGE_WIDTH, IMAGE_HEIGHT, 2, [](){stop.notify_all();});

	cam->start();

	if(isPreviewVisible)
	{
		std::mutex m;
		std::unique_lock<std::mutex> lk(m);
		stop.wait(lk);
		lk.unlock();
	}
	else
	{
		std::cout << "\"exit\" stops the programm" << std::endl;
		while (true)
		{
			std::string line;
			std::getline(std::cin, line);
			if(line == "exit")
				break;
		}
	}

	if(isPreviewVisible)
		window->close();

	// close all
	Controller::getInstance()->stop();
	cam->stop();
	if(isPreviewVisible)
		delete window;
	return 0;
}
