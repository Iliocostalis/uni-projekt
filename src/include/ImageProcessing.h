#pragma once
#include <stdint.h>
#include <stdlib.h>
#include <X11/Xlib.h>

#include <array>
#include <vector>
#include <atomic>

#include <Config.h>
#include <Utils.h>

namespace ImageProcessing
{
    extern std::array<std::vector<uint8_t>, IMAGE_BUFFER_COUNT> imageBuffer;
    extern std::atomic<int> currentImageIndex;

    extern void init();
    extern void process(uint8_t* data, size_t size);
    extern void findLines(std::vector<uint8_t>& previewImage, uint8_t* data, size_t size);

    extern void calculateSteering(std::vector<uint8_t>& previewImage, const std::vector<Position<int>>& pointsOnLineLeft, const std::vector<Position<int>>& pointsOnLineRight);

    extern void saveImage();
    extern void saveVideo();
    extern void stopVideo();

    extern void saveImageToFolder(const std::string& fileName, uint8_t* data);
    extern void readImageFromFolder(const std::string& fileName, std::vector<uint8_t>* imageData);
}
