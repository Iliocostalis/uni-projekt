#include "imageProcessing.h"
#include <iostream>
#include <fstream>
#include <string>

namespace ImageProcessing
{
    int val = 0;
    
    void process(uint8_t* data, size_t size)
    {
        std::cout << "image count: " << val << std::endl;

        if(val == 4)
        {
            std::string fileName = std::string("image_") + std::to_string(val);
            
            auto myfile = std::ofstream(fileName, std::ios::out | std::ios::binary);
            myfile << "P6" << "\n" << 640 << " " << 480 << "\n" << 255 << "\n";
            myfile.write((char*)data, size);
            myfile.close();
        }

        ++val;
    }
}