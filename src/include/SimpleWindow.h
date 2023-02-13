#ifndef SIMPLE_WINDOW_H
#define SIMPLE_WINDOW_H

#include <vector>
#include <stdint.h>
#include <iostream>
#include <thread>
#include <atomic>
#include <functional>
#include <chrono>
#ifdef SIMPLE_WINDOW_WINDOWS
#define NOMINMAX
#include <Windows.h>
#else
#include <X11/Xlib.h>
#endif


#ifdef SIMPLE_WINDOW_WINDOWS
LRESULT CALLBACK WindowProcessMessages(HWND hwnd, UINT msg, WPARAM param, LPARAM lparam);
#endif

class SimpleWindow
{
private:
    const uint32_t width;
    const uint32_t height;
    const uint32_t imageCount;

#ifdef SIMPLE_WINDOW_WINDOWS
    typedef std::chrono::system_clock::time_point timePoint;
    HWND window;
    HDC hdc;
    std::thread windowThread;
#else
    typedef std::chrono::system_clock::time_point timePoint;
    Atom del_window;
    Display* display;
    Window window;
    int screen;
    GC gc;
#endif

    std::atomic<uint32_t> imageIndex;
    std::atomic<uint32_t> imageIndexNext;

#ifdef SIMPLE_WINDOW_WINDOWS
    std::vector<HBITMAP> images;
    std::vector<HDC> imagesMemory;
    std::vector<HBITMAP> bitmaps;
#else
    std::vector<XImage*> images;
    std::vector<uint8_t> imagePixels;
#endif
    std::vector<uint8_t*> pToPixels;

    std::atomic_bool isWindowOpening;
    std::atomic_bool isWindowOpen;
    std::atomic_bool isAutoRefreshEnabled;
    std::atomic_bool isLoopRunning;
    std::thread autoRefreshThread;
    timePoint lastRefreshed;
    std::function<void(void)> closeCallback;

#ifdef SIMPLE_WINDOW_WINDOWS
    void windowLoop(HINSTANCE currentInstance, const uint32_t width, const uint32_t height, const uint32_t imageCount);
#else
    void handleEvents();
#endif
    void refreshLoop(const uint32_t refreshRate);
    void refreshScreen();

public:
    SimpleWindow(const uint32_t width, const uint32_t height, const uint32_t imageCount, const std::function<void(void)>& closeCallback = nullptr);
    ~SimpleWindow();

    void startAutoRefresh(const uint32_t refreshRate);
    void stopAutoRefresh();
    void swap();
    void swap(const uint32_t index);
    void swapAtRefreshRate(const uint32_t refreshRate);
    void setImageIndex(const uint32_t index);
    uint8_t* getImagePointer(const uint32_t index);
    uint32_t getNextImageIndex();

    void close();
    bool isOpen();
    void waitTillClosed();
};

#endif //SIMPLE_WINDOW_H
#ifdef SIMPLE_WINDOW_IMPLEMENTATION

#ifdef SIMPLE_WINDOW_WINDOWS
SimpleWindow::SimpleWindow(const uint32_t width, const uint32_t height, const uint32_t imageCount, const std::function<void(void)>& closeCallback/* = nullptr*/) : width(width), height(height), imageCount(imageCount), isAutoRefreshEnabled(false), isLoopRunning(false), isWindowOpen(false), isWindowOpening(true)
{
    this->closeCallback = closeCallback;
    windowThread = std::thread(std::bind(&SimpleWindow::windowLoop, this, GetModuleHandle(NULL), width, height, imageCount));
    
    auto timeBegin = std::chrono::high_resolution_clock::now();
    while(isWindowOpening)
    {
        auto timeNow = std::chrono::high_resolution_clock::now();
        uint32_t millis = std::chrono::duration_cast<std::chrono::milliseconds>(timeNow-timeBegin).count();
        if(millis > 1000)
        {
            std::cout << "window can not open" << std::endl;
            return;
        }    
    };
}
#else
SimpleWindow::SimpleWindow(const uint32_t width, const uint32_t height, const uint32_t imageCount, const std::function<void(void)>& closeCallback/* = nullptr*/) : width(width), height(height), imageCount(imageCount), isAutoRefreshEnabled(false), isLoopRunning(false), isWindowOpen(false), isWindowOpening(true)
{
    this->closeCallback = closeCallback;
    imageIndex = 0;
    imageIndexNext = (imageIndex+1)%imageCount;
    lastRefreshed = timePoint::min();

    display = XOpenDisplay(NULL);
    if (display == NULL) 
	{
        std::cout << "Cannot open display" << std::endl;
		return;
    }


    images.resize(imageCount);
    imagePixels.resize(width*height*4*imageCount);
    pToPixels.resize(imageCount);
    for(int i = 0; i < imageCount; ++i)
        pToPixels[i] = &imagePixels[width*height*4*i];


    screen = DefaultScreen(display);

    const int border = 1;
    window = XCreateSimpleWindow(display, RootWindow(display, screen), 0, 0, width, height, border, BlackPixel(display, screen), WhitePixel(display, screen));

    /* process window close event through event handler so XNextEvent does not fail */
    del_window = XInternAtom(display, "WM_DELETE_WINDOW", 0);
    XSetWMProtocols(display, window, &del_window, 1);

    /* select kind of events we are interested in */
    XSelectInput(display, window, ExposureMask | KeyPressMask);

    /* display the window */
    XMapWindow(display, window);

    XFlush(display);
    gc = XCreateGC(display, window, 0, NULL);

	for(int i = 0; i < imageCount; ++i)
		images[i] = XCreateImage(display, DefaultVisual(display, 0), 24, ZPixmap, 0, (char*)pToPixels[i], width, height, 32, 0);

    isWindowOpen = true;
    isWindowOpening = false;
}
#endif //SIMPLE_WINDOW_WINDOWS

SimpleWindow::~SimpleWindow()
{
    close();
    if(autoRefreshThread.joinable())
        autoRefreshThread.join();
#ifdef SIMPLE_WINDOW_WINDOWS
    if(windowThread.joinable())
        windowThread.join();
#endif
}

void SimpleWindow::refreshLoop(const uint32_t refreshRate)
{
    const uint32_t time = 1000000 / refreshRate;
    while(isAutoRefreshEnabled)
    {
        auto timeBegin = std::chrono::high_resolution_clock::now();

        refreshScreen();

        auto timeEnd = std::chrono::high_resolution_clock::now();

        uint32_t sleepMicro = time - std::chrono::duration_cast<std::chrono::microseconds>(timeEnd-timeBegin).count();
        sleepMicro = std::min(time, sleepMicro);

        std::this_thread::sleep_for(std::chrono::microseconds(sleepMicro));
    }
    isLoopRunning = false;
}

void SimpleWindow::startAutoRefresh(const uint32_t refreshRate)
{
    if(isLoopRunning)
        return;

    if(refreshRate == 0)
        return;

    isAutoRefreshEnabled = true;
    isLoopRunning = true;
    autoRefreshThread = std::thread(std::bind(&SimpleWindow::refreshLoop, this, std::placeholders::_1), refreshRate);
}

void SimpleWindow::stopAutoRefresh()
{
    isAutoRefreshEnabled = false;
}

bool SimpleWindow::isOpen()
{
    return isWindowOpen;
}

void SimpleWindow::waitTillClosed()
{
    if(autoRefreshThread.joinable())
        autoRefreshThread.join();
}

void SimpleWindow::setImageIndex(const uint32_t index)
{
    imageIndex = std::min(index, imageCount - 1);
    imageIndexNext = (imageIndex+1)%imageCount;
}

uint8_t* SimpleWindow::getImagePointer(const uint32_t index)
{
    return pToPixels[index];
}

void SimpleWindow::refreshScreen()
{
#ifdef SIMPLE_WINDOW_WINDOWS
    if(!isWindowOpen)
        return;

    BitBlt(hdc, 0, 0, width, height, imagesMemory[imageIndex], 0, 0, SRCCOPY);
    GdiFlush();
    SelectObject(imagesMemory[imageIndex], images[imageIndex]);
#else
    handleEvents();
    if(!isWindowOpen)
        return;

    XPutImage(display, window, gc, images[imageIndex], 0, 0, 0, 0, width, height);
    XFlush(display);
#endif
}

void SimpleWindow::swap()
{
    imageIndex = imageIndexNext.load();
    imageIndexNext = (imageIndexNext+1)%imageCount;

    refreshScreen();
}

void SimpleWindow::swap(const uint32_t index)
{
    imageIndex = index;
    imageIndexNext = (imageIndex+1)%imageCount;

    refreshScreen();
}

uint32_t SimpleWindow::getNextImageIndex()
{
    return imageIndexNext;
}

void SimpleWindow::swapAtRefreshRate(const uint32_t refreshRate)
{
    const uint32_t time = 1000000 / refreshRate;

    swap();

    auto timeEnd = std::chrono::system_clock::now();

    uint32_t sleepMicro = time - std::chrono::duration_cast<std::chrono::microseconds>(timeEnd-lastRefreshed).count();
    lastRefreshed = timeEnd;

    sleepMicro = std::min(time, sleepMicro);
    std::this_thread::sleep_for(std::chrono::microseconds(sleepMicro));
}

#ifdef SIMPLE_WINDOW_WINDOWS
void SimpleWindow::windowLoop(HINSTANCE currentInstance, const uint32_t width, const uint32_t height, const uint32_t imageCount)
{
    imageIndex = 0;
    imageIndexNext = (imageIndex+1)%imageCount;
    lastRefreshed = timePoint::min();

    images.resize(imageCount);
    imagesMemory.resize(imageCount);
    pToPixels.resize(imageCount);
    bitmaps.resize(imageCount);

    const char* CLASS_NAME = "myWin32WindowClass";
    WNDCLASS wc{};
    wc.hInstance = currentInstance;
    wc.lpszClassName = CLASS_NAME;
    wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)COLOR_WINDOW;
    wc.lpfnWndProc = WindowProcessMessages;
    RegisterClass(&wc);

    window = CreateWindow(CLASS_NAME, "Win32 Window", WS_OVERLAPPEDWINDOW | WS_VISIBLE, CW_USEDEFAULT, CW_USEDEFAULT, width, height+39, nullptr, nullptr, nullptr, nullptr);

    hdc = GetDC(window);

    BITMAPINFO bInfo{};
    bInfo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bInfo.bmiHeader.biWidth = width;
    bInfo.bmiHeader.biHeight = -height;
    bInfo.bmiHeader.biPlanes = 1;
    bInfo.bmiHeader.biBitCount = 32;
    bInfo.bmiHeader.biCompression = BI_RGB;
    bInfo.bmiHeader.biSizeImage = 0;
    bInfo.bmiHeader.biXPelsPerMeter = 0;
    bInfo.bmiHeader.biYPelsPerMeter = 0;
    bInfo.bmiHeader.biClrUsed = 0;
    bInfo.bmiHeader.biClrImportant = 0;

    for(int i = 0; i < imageCount; ++i)
    {
        imagesMemory[i] = CreateCompatibleDC(hdc);
        bitmaps[i] = CreateDIBSection(imagesMemory[i], &bInfo, DIB_RGB_COLORS, (void**)&pToPixels[i], NULL, 0);
        GdiFlush();
        (HBITMAP)SelectObject(imagesMemory[i], bitmaps[i]);
    }

    isWindowOpen = true;
    isWindowOpening = false;

    MSG msg{};
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    close();
}
#else
void SimpleWindow::handleEvents()
{
    if(!isWindowOpen)
        return;

    while(XEventsQueued(display, QueuedAlready) > 0)
    {
        XEvent event{};
        XNextEvent(display, &event);
        if(event.type = ClientMessage && event.xclient.data.l[0] == del_window)
        {
            close();
            continue;
        }
    }
}
#endif

void SimpleWindow::close()
{
    if(!isWindowOpen)
        return;

    if(closeCallback != NULL)
        closeCallback();
        
    isAutoRefreshEnabled = false;
    isWindowOpen = false;
    isWindowOpening = false;

#ifdef SIMPLE_WINDOW_WINDOWS
    SendMessage(window, WM_DESTROY, 0, 0);
    ReleaseDC(window, hdc);
#else
    XDestroyWindow(display, window);
    XCloseDisplay(display);
#endif
}

#ifdef SIMPLE_WINDOW_WINDOWS
LRESULT CALLBACK WindowProcessMessages(HWND hwnd, UINT msg, WPARAM param, LPARAM lparam)
{
    switch (msg)
    {
    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;
    default: 
        return DefWindowProc(hwnd, msg, param, lparam);
    }
}
#endif
#endif // SIMPLE_WINDOW_IMPLEMENTATION