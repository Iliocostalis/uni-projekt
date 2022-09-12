#include <Preview.h>
#include <ImageProcessing.h>
#include <iostream>
#include <chrono>
#include <atomic>
#include <thread>

Preview::Preview(){}

Preview* Preview::getInstance()
{
    static Preview preview;
    return &preview;
}

std::atomic_bool isPreviewLoopRunning(false);
void* previewLoop(void* arg)
{
	auto start = std::chrono::high_resolution_clock::now();
	auto last = std::chrono::high_resolution_clock::now();
    while(isPreviewLoopRunning)
    {
		auto now = std::chrono::high_resolution_clock::now();
        auto milliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(now - last);

		// limit to 20 fps
		if(milliseconds.count() >= 50)
		{
			last = now;

#if DEFINED(PREVIEW_LOG)
			std::cout << "refresh window" << std::endl;
			std::cout << "image index: " << ImageProcessing::currentImageIndex << std::endl;
#endif
			Preview::getInstance()->refresh();
		}
		std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
    return (void*)nullptr;
}

void Preview::open()
{
    display = XOpenDisplay(NULL);
    if (display == NULL) 
	{
        std::cout << "Cannot open display" << std::endl;
		exit(1);
    }

    screen = DefaultScreen(display);

    window = XCreateSimpleWindow(display, RootWindow(display, screen), WINDOW_X, WINDOW_Y, WINDOW_WIDTH, WINDOW_HEIGHT, WINDOW_BORDER, BlackPixel(display, screen), WhitePixel(display, screen));

    /* process window close event through event handler so XNextEvent does not fail */
    Atom del_window = XInternAtom(display, "WM_DELETE_WINDOW", 0);
    XSetWMProtocols(display, window, &del_window, 1);

    /* select kind of events we are interested in */
    XSelectInput(display, window, ExposureMask | KeyPressMask);

    /* display the window */
    XMapWindow(display, window);

    XFlush(display);
    gc = XCreateGC(display, window, 0, NULL);

	for(int i = 0; i < IMAGE_BUFFER_COUNT; ++i)
		images[i] = XCreateImage(display, DefaultVisual(display, 0), 24, ZPixmap, 0, ImageProcessing::imageBuffer[i].data(), IMAGE_WIDTH, IMAGE_HEIGHT, 32, 0);


    isPreviewLoopRunning = true;
    int ret = pthread_create(&previewLoopThread, NULL, &previewLoop, NULL);
}

void Preview::refresh()
{
    XPutImage(display, window, gc, images[ImageProcessing::currentImageIndex], 0, 0, 0, 0, IMAGE_WIDTH, IMAGE_HEIGHT);
    XFlush(display);
}

void Preview::close()
{
    isPreviewLoopRunning = false;
    void* returnValue;
	pthread_join(previewLoopThread, &returnValue);

    XDestroyWindow(display, window);
    XCloseDisplay(display);
}