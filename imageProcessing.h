#pragma once
#include <stdint.h>
#include <stdlib.h>
#include <X11/Xlib.h>

#include <array>
#include <vector>
#include <atomic>

#include "config.h"

namespace ImageProcessing
{
    extern std::array<std::vector<char>, IMAGE_BUFFER_COUNT> imageBuffer;
    extern std::atomic<int> currentImageIndex = 0;
    extern std::array<XImage*, IMAGE_BUFFER_COUNT> images;

    extern void init();
    extern void process(uint8_t* data, size_t size);
}