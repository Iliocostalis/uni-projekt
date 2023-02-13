#pragma once
#include <stdint.h>
#include <stdlib.h>

#include <array>
#include <vector>
#include <atomic>

#include <Config.h>
#include <Utils.h>

namespace ImageProcessing
{
    extern void process(uint8_t* image, size_t size);
    extern void findLinesNew(uint8_t* previewImage, uint8_t* image, std::vector<Position<int>>* lineLeft, std::vector<Position<int>>* lineRight);
    extern double getPercentageDarkPixels(uint8_t* image, uint8_t* previewImage, int colorThresholdDark);

    extern void calculateSteering(uint8_t* previewImage, const std::vector<Position<int>>& pointsOnLineLeft, const std::vector<Position<int>>& pointsOnLineRight);

    extern void saveVideo();
    extern void stopVideo();

    extern void saveImageToFolder(const std::string& fileName, uint8_t* data);
    extern void readImageFromFolder(const std::string& fileName, std::vector<uint8_t>* imageData);
}
