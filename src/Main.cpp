#include <iostream>
#include <Config.h>
#include <Cam.h>
#include <ImageProcessing.h>
#include <Preview.h>
#include <Controller.h>

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

    while(true)
    {
		std::string line;
		std::getline( std::cin, line );

		//std::cout << line << std::endl;
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

	// close all

#if DEFINED(SHOW_PREVIEW)
    Preview::getInstance()->close();
#endif

	Cam::getInstance()->stop();
	
	return 0;
}
