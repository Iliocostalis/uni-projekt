#include <array>
#include <X11/Xlib.h>
#include <Config.h>

class Preview
{
    Display* display;
    Window window;
    XEvent event;
    int screen;
    GC gc;
    std::array<XImage*, IMAGE_BUFFER_COUNT> images;

    Preview();
public:
    Preview(Preview const&)         = delete;
    void operator=(Preview const&)  = delete;

    static Preview* getInstance();
    void open();
    void refresh();
    void close();
};