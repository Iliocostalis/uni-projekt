#pragma once
#include <stdint.h>
#include <stdlib.h>
#include <X11/Xlib.h>

#include <array>
#include <vector>
#include <atomic>

#include <Config.h>

namespace ImageProcessing
{
    extern std::array<std::vector<char>, IMAGE_BUFFER_COUNT> imageBuffer;
    extern int currentImageIndex;

    extern void init();
    extern void process(uint8_t* data, size_t size);
}