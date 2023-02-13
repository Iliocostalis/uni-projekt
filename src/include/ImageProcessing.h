#pragma once
#include <stdint.h>
#include <vector>
#include <Config.h>
#include <Utils.h>

namespace ImageProcessing
{
    extern void process(uint8_t* image, size_t size);
    
    extern void saveVideo();
    extern void stopVideo();

    extern void saveImageToFolder(const std::string& fileName, uint8_t* data);
    extern void readImageFromFolder(const std::string& fileName, std::vector<uint8_t>* imageData);
}
