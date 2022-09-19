#include <Preview.h>
#include <ImageProcessing.h>
#include <iostream>
#include <chrono>
#include <atomic>
#include <thread>
#include <algorithm>

Preview::Preview(){}

Preview* Preview::getInstance()
{
    static Preview preview;
    return &preview;
}

std::atomic_bool isPreviewLoopRunning(false);
void* previewLoop(void* arg)
{
    while(isPreviewLoopRunning)
    {
		auto now = std::chrono::high_resolution_clock::now();

#if DEFINED(PREVIEW_LOG)
        std::cout << "refresh window" << std::endl;
        std::cout << "image index: " << ImageProcessing::currentImageIndex << std::endl;
#endif
        Preview::getInstance()->refresh();

        // limit to 20 fps
	    auto last = std::chrono::high_resolution_clock::now();
        auto microseconds = std::chrono::duration_cast<std::chrono::microseconds>(last - now);
        
        int sleep = std::max(0, 50000 - (int)microseconds.count());
		std::this_thread::sleep_for(std::chrono::microseconds(sleep));
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
		images[i] = XCreateImage(display, DefaultVisual(display, 0), 24, ZPixmap, 0, (char*)ImageProcessing::imageBuffer[i].data(), IMAGE_WIDTH, IMAGE_HEIGHT, 32, 0);


    isPreviewLoopRunning = true;
    int ret = pthread_create(&previewLoopThread, NULL, &previewLoop, NULL);
}

void Preview::refresh()
{
    while(XEventsQueued(display, QueuedAlready) > 0)
    {
        XEvent event{};
        XNextEvent(display, &event);
        if(event.type = ClientMessage && event.xclient.message_type == 288)
        {
            stop.notify_all();
            isPreviewLoopRunning = false;
            return;
        }
    }
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