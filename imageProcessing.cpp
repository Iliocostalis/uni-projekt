#include "imageProcessing.h"
#include <iostream>
#include <fstream>
#include <string>
#include <chrono>

namespace ImageProcessing
{
    int val = 0;
    std::chrono::time_point<std::chrono::system_clock> last = std::chrono::high_resolution_clock::now();

    void process(uint8_t* data, size_t size)
    {
        auto now = std::chrono::high_resolution_clock::now();
        auto seconds = std::chrono::duration_cast<std::chrono::seconds>(now - last);
        last = now;


        std::cout << "image count: " << val << std::endl;
        std::cout << "fps: " << 1.0f / seconds.count() << std::endl;
        std::cout << "size: " << size << std::endl;

        if(val == 30)
        {
            std::string fileName = std::string("image_") + std::to_string(val);
            
            auto myfile = std::ofstream(fileName, std::ios::out | std::ios::binary);
            myfile << "P6" << "\n" << 640 << " " << 480 << "\n" << 255 << "\n";
            size = 640 * 480 / 2;
            for(int i = 0; i < size; ++i)
            {
                uint8_t valA = (data[i] & 0xf0) >> 4;
                uint8_t valB = (data[i] & 0x0f);

                myfile.write((char*)&valA, 1);
                myfile.write((char*)&valA, 1);
                myfile.write((char*)&valA, 1);
                myfile.write((char*)&valB, 1);
                myfile.write((char*)&valB, 1);
                myfile.write((char*)&valB, 1);
            }
            myfile.close();
        }

        ++val;
    }
}