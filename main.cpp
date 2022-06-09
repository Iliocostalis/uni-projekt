#include <iostream>
#include <cam.h>

int main()
{
	Cam cam;
	cam.init();
	cam.start();
	cam.stop();
	
	return 0;
}
