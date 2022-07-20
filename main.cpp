#include <iostream>
#include <iomanip>
#include <thread>

#include <stdio.h>
#include <stdlib.h>

#include "config.h"
#include "cam.h"
#include "imageProcessing.h"

int main()
{
	ImageProcessing::init();

	Display *display;
    Window window;
    XEvent event;
    int screen;

    /* open connection with the server */
    display = XOpenDisplay(NULL);
    if (display == NULL) 
	{
		fprintf(stderr, "Cannot open display\n");
		exit(1);
    }

    screen = DefaultScreen(display);

    /* create window */
    window = XCreateSimpleWindow(display, RootWindow(display, screen), WINDOW_X, WINDOW_Y, WINDOW_WIDTH, WINDOW_HEIGHT, WINDOW_BORDER, BlackPixel(display, screen), WhitePixel(display, screen));

    /* process window close event through event handler so XNextEvent does not fail */
    Atom del_window = XInternAtom(display, "WM_DELETE_WINDOW", 0);
    XSetWMProtocols(display, window, &del_window, 1);

    /* select kind of events we are interested in */
    XSelectInput(display, window, ExposureMask | KeyPressMask);

    /* display the window */
    XMapWindow(display, window);

    XFlush(display);
    GC gc = XCreateGC(display, window, 0, NULL);

	for(int i = 0; i < IMAGE_BUFFER_COUNT; ++i)
		ImageProcessing::images[i] = XCreateImage(display, DefaultVisual(display, 0), 24, ZPixmap, 0, ImageProcessing::imageBuffer[i].data(), IMAGE_WIDTH, IMAGE_HEIGHT, 32, 0);




	Cam cam;
	cam.init();
	cam.start();
	std::cout << "func finished" << std::endl;

	auto start = std::chrono::high_resolution_clock::now();
	auto last = std::chrono::high_resolution_clock::now();
    while(true)
    {
		auto now = std::chrono::high_resolution_clock::now();
        auto milliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(now - last);
        auto runningTime = std::chrono::duration_cast<std::chrono::seconds>(now - start);

		if(runningTime.count() >= 3)
			break;

		if(milliseconds.count() > 50)
		{
			last = now;
			std::cout << "refresh window" << std::endl;
			std::cout << "image index: " << ImageProcessing::currentImageIndex << std::endl;
			XPutImage(display, window, gc, ImageProcessing::images[ImageProcessing::currentImageIndex], 0, 0, 0, 0, IMAGE_WIDTH, IMAGE_HEIGHT);
    		XFlush(display);
		}
    }
	cam.stop();

    /* event loop 
    while (1) {
            XNextEvent(display, &event);

            switch (event.type) {
                    case KeyPress:
                            // FALLTHROUGH
                    case ClientMessage:
                            goto breakout;
                    case Expose:
                            XPutImage(display, window, gc, image, 0, 0, 0, 0, width, height);



            }
    }*/
breakout:

    /* destroy window */
    XDestroyWindow(display, window);

    /* close connection to server */
    XCloseDisplay(display);
	


	//std::this_thread::sleep_for(std::chrono::seconds(3));

	//cam.stop();
	
	return 0;
}
