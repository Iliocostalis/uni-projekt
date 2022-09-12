#include <ImageProcessing.h>
#include <iostream>
#include <fstream>
#include <string>
#include <chrono>

namespace ImageProcessing
{
    int imageNameCount = 0;
    std::atomic_bool saveImageBool(false);
    std::atomic_bool saveVideoBool(false);
    auto lastImageSaved = std::chrono::high_resolution_clock::now();
    auto last = std::chrono::high_resolution_clock::now();

    std::chrono::microseconds lasts[5];
    int ind = 0;

    std::array<std::vector<char>, IMAGE_BUFFER_COUNT> imageBuffer;
    std::atomic<int> currentImageIndex(0);

    void init()
    {
#if DEFINED(SHOW_PREVIEW)
        for(auto& image : imageBuffer)
            image.resize(IMAGE_WIDTH * IMAGE_HEIGHT * 4);
#endif
    }

    void process(uint8_t* data, size_t size)
    {
#if DEFINED(SHOW_PREVIEW)
        int nextImageIndex = (currentImageIndex + 1) % imageBuffer.size();
        for(int i = 0; i < IMAGE_WIDTH * IMAGE_HEIGHT; ++i)
        {
            imageBuffer[nextImageIndex][i * 4 + 0] = data[i];
            imageBuffer[nextImageIndex][i * 4 + 1] = data[i];
            imageBuffer[nextImageIndex][i * 4 + 2] = data[i];
            imageBuffer[nextImageIndex][i * 4 + 3] = data[i];
        }
        currentImageIndex = nextImageIndex;
#endif

        auto now = std::chrono::high_resolution_clock::now();
        auto microseconds = std::chrono::duration_cast<std::chrono::microseconds>(now - last);
        last = now;

#if DEFINED(CAMERA_LOG)
        lasts[ind] = microseconds;
        ind = (ind + 1) % 5;

        int64_t averageTime = 0;
        for(int i = 0; i < 5; ++i)
            averageTime += lasts[i].count();

        averageTime /= 5;

        std::cout << "image count: " << val << std::endl;
        std::cout << "fps: " << 1000000.0f / averageTime << std::endl;
        std::cout << "size: " << size << std::endl;
#endif
        auto timeLastImageSaved = std::chrono::duration_cast<std::chrono::milliseconds>(now - lastImageSaved);

        // Y420
        if(saveImageBool || (saveVideoBool && timeLastImageSaved.count() > 500))
        {
            lastImageSaved = now;
            saveImageBool = false;
            std::string fileName = std::string("../images/image_") + std::to_string(imageNameCount);
            imageNameCount += 1;

            saveImageToFolder(fileName, data);
        }

        findLines(data, size);
    }

    void findLines(uint8_t* data, size_t size)
    {
        int posX = IMAGE_WIDTH / 2;
        int posY = (IMAGE_HEIGHT / 3) * 2;
        int averageColor = 0;
        for(int y = -4; y <= 4; ++y)
        {
            for(int x = -4; x <= 4; ++x)
            {
                int ix = posX + x;
                int iy = posY + y;
                averageColor += (int)data[iy * IMAGE_WIDTH + ix];
            }
        }
        averageColor /= 81;

        
    }

    void calculateSteering(const std::vector<Position>& pointsOnLineLeft, const std::vector<Position>& pointsOnLineRight)
    {
        Car car{};
        car.steering = 0;
        car.throtle = 0;

        // TODO
    }
    
    void saveImage()
    {
        saveImageBool = true;
    }
    
    void saveVideo()
    {
        saveVideoBool = true;
    }
    
    void stopVideo()
    {
        saveVideoBool = false;
    }

    void saveImageToFolder(const std::string& fileName, uint8_t* data)
    {
        auto myfile = std::ofstream(fileName, std::ios::out | std::ios::binary);
        myfile << "P6" << "\n" << IMAGE_WIDTH << " " << IMAGE_HEIGHT << "\n" << 255 << "\n";
        int imageSize = IMAGE_WIDTH * IMAGE_HEIGHT;
        for(int i = 0; i < imageSize; ++i)
        {
            uint8_t valA = data[i];
            myfile.write((char*)&valA, 1);
            myfile.write((char*)&valA, 1);
            myfile.write((char*)&valA, 1);
        }
        myfile.close();
        std::cout << "image saved" << std::endl;
    }

    void readImageFromFolder(const std::string& fileName, std::vector<uint8_t>* imageData)
    {
        auto file = std::ifstream(fileName, std::ios::in | std::ios::binary);

        if(!file.is_open())
            return;

        int width;
        int height;
        std::string line;

        std::getline(file, line);

        if(line != "P6")
            return;

        std::getline(file, line, ' ');
        width = std::stoi(line);
        std::getline(file, line);
        height = std::stoi(line);
        std::getline(file, line);

        // header complete
        int imageSize = width*height;
        int inputSize = imageSize*3;

        char* input = new char[inputSize];

        file.read(input, inputSize);
        file.close();

        imageData->resize(imageSize);
        uint8_t* imageOut = imageData->data();

        int indexInput = 0;
        for(int i = 0; i < imageSize; ++i)
        {
            imageOut[i] = (uint8_t)input[indexInput];
            indexInput += 3;
        }

        delete[] input;
    }
}
