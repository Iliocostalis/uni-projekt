#include <ImageProcessing.h>
#include <iostream>
#include <fstream>
#include <string>
#include <chrono>
#include <cmath>
#include <Debugging.h>

namespace ImageProcessing
{
    int imageNameCount = 0;
    std::atomic_bool saveImageBool(false);
    std::atomic_bool saveVideoBool(false);
    auto lastImageSaved = std::chrono::high_resolution_clock::now();
    auto last = std::chrono::high_resolution_clock::now();

    Average<std::chrono::microseconds> averageTime(5);
    std::chrono::microseconds lasts[5];

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
        findLines(data, size);

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
        averageTime.addSample(microseconds);
        std::cout << "image count: " << averageTime.getAverage().count() << std::endl;
        std::cout << "fps: " << 1000000.0f / averageTime.getAverage().count() << std::endl;
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
    }

    int blackCount(uint8_t* data, const std::vector<Position<int>>& circleOffsets, const Position<int>& pos, int threshold)
    {
        int sum = 0;
        for(auto& offset : circleOffsets)
        {
            if(data[(pos.y + offset.y) * IMAGE_WIDTH + pos.x + offset.x] < threshold)
                sum += 1;
        }
        return sum;
    }

    Position<int> moveTillBorder(uint8_t* data, const Position<int>& start, int moveX, int colorThresholdDark, std::vector<uint8_t>& imageOut)
    {
        ASSERT(moveX == 1 || moveX == -1)

        Position<int> pos(start);

        int sum = 0;
        int min = 255;
        int max = 0;
        for(int x = start.x; x >= 0 && x < IMAGE_WIDTH;)
        {
            int val = (int)data[start.y * IMAGE_WIDTH + x];
            min = std::min(min, val);
            max = std::max(max, val);

            if(val < colorThresholdDark)
            {
                sum += 1;
            }
            else
            {
#if DEFINED(DEBUG)
                imageOut[start.y * IMAGE_WIDTH + x] = 255;
#endif
                sum = std::max(0, sum - 2);
            }

            if(sum >= 3)
            {
                pos.x = x - 3*moveX;
                break;
            }

            x += moveX;
        }

        return pos;
    }

    std::vector<Position<int>> followLine(uint8_t* data, const Position<int>& firstPoint, const Position<int>& secondPoint, int move, int colorThresholdDark, const std::vector<Position<int>>& circleOffsets, std::vector<uint8_t>& imageOut)
    {
        ASSERT(move > 0)

        float moveF = move;
        Position<float> last(firstPoint);
        Position<float> current(secondPoint);
        Position<float> next;

        std::vector<Position<int>> line;

        Average<Position<float>> averageMove(3);

        int maxSteps = 1000;
        int step = 0;
        int failed = 0;
        while(step < maxSteps)
        {
            float bestOffset = 0;
            int lowest = 18;

            averageMove.addSample(current - last);
            Position<float> move = averageMove.getAverage();

            float moveScale = std::max(std::abs(move.x), std::abs(move.y));

            next = current + (move / moveScale) * moveF;
            if(next.x <= 3 || next.x >= IMAGE_WIDTH - 3 || next.y <= 3 || next.y >= IMAGE_HEIGHT - 3)
                break;

            Position<float> m;
            m.x = -(next.y - current.y);
            m.y = next.x - current.x;

            float scale = std::max(std::abs(m.x), std::abs(m.y));
            m = m / scale;

            for(float o = -2.0f; o <= 2.0f; ++o)
            {
                //auto pp = next + m * o + Position<float>(0.5f, 0.5f);
                int b = blackCount(data, circleOffsets, next + m * o + Position<float>(0.5f, 0.5f), colorThresholdDark);
                int diff = std::abs(b-9);
                if(diff*3 + (int)std::abs(o) < lowest*3)
                {
                    bestOffset = o;
                    lowest = diff;
                }
            }

            if(lowest > 7)
            {
                failed += 1;
                if(failed > 2)
                    break;
            }

            last = current;
            current = next + m * bestOffset;

            line.push_back(current + Position<float>(0.5f, 0.5f));
#if DEFINED(DEBUG)
            imageOut[line.back().y * IMAGE_WIDTH + line.back().x] = 255;
#endif
            step += 1;
        }

        return line;
    }

    void findLines(uint8_t* data, size_t size)
    {
#if DEFINED(DEBUG)
        std::vector<uint8_t> imageOut;
        imageOut.resize(size);
        std::copy(data, data+size, imageOut.data());
#endif

        std::vector<Position<int>> circleOffsets;
        int space = 2;
        for(int i = 0; i < 18; ++i)
        {
            float rad = (float)((360 / 18) * i) * (M_PI / 180);
            Position<int> pos;
            pos.x = (int)(std::cos(rad) * space + 10.5f) - 10;
            pos.y = (int)(std::sin(rad) * space + 10.5f) - 10;
            circleOffsets.push_back(pos);
        }

        Position<int> start;
        start.x = IMAGE_WIDTH / 2;
        start.y = (IMAGE_HEIGHT / 8) * 7;

        int averageColor = 0;
        for(int y = -4; y <= 4; ++y)
        {
            for(int x = -4; x <= 4; ++x)
            {
                int ix = start.x + x;
                int iy = start.y + y;
                averageColor += (int)data[iy * IMAGE_WIDTH + ix];
            }
        }
        averageColor /= 81;
        int colorThresholdDark = averageColor * 0.8f;


        Position<int> leftFirst = moveTillBorder(data, start, -1, colorThresholdDark, imageOut);
        Position<int> leftSecond = moveTillBorder(data, leftFirst+Position<int>(20, -3), -1, colorThresholdDark, imageOut);
        Position<int> rightFrist = moveTillBorder(data, start, 1, colorThresholdDark, imageOut);
        Position<int> rightSecond = moveTillBorder(data, rightFrist+Position<int>(-20, -3), 1, colorThresholdDark, imageOut);

        auto lineLeft = followLine(data, leftFirst, leftSecond, 4, colorThresholdDark, circleOffsets, imageOut);
        auto lineRight = followLine(data, rightFrist, rightSecond, 4, colorThresholdDark, circleOffsets, imageOut);


#if DEFINED(DEBUG)
        std::copy(imageOut.data(), imageOut.data()+size, data);

        static bool runOnce = true;
        if(runOnce)
        {
            saveImageToFolder("../tmpImage", imageOut.data());
            runOnce = false;
        }
#endif
    }

    void calculateSteering(const std::vector<Position<int>>& pointsOnLineLeft, const std::vector<Position<int>>& pointsOnLineRight)
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
