#include <iostream>
#include <cam.h>
#include <iomanip>
#include <thread>

int main()
{
	Cam cam;
	cam.init();
	cam.start();


	//std::this_thread::sleep_for(std::chrono::seconds(3));

	//cam.stop();
	
	return 0;
}
