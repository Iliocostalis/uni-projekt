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

    std::array<std::vector<uint8_t>, IMAGE_BUFFER_COUNT> imageBuffer;
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
            imageBuffer[nextImageIndex][i * 4 + 3] = 255;
        }
        currentImageIndex = nextImageIndex;
#endif

        findLines(imageBuffer[nextImageIndex], data, size);

#if DEFINED(SHOW_PREVIEW)
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
        if(saveImageBool || (saveVideoBool && timeLastImageSaved.count() > SAVE_VIDEO_FRAME_DELAY))
        {
            lastImageSaved = now;
            saveImageBool = false;
            std::string fileName = std::string("../images/image_") + std::to_string(imageNameCount);
            imageNameCount += 1;

            saveImageToFolder(fileName, data);
        }
    }

    inline void writePreviewPixel(std::vector<uint8_t>& previewImage, int x, int y, int rgb, uint8_t value)
    {
        ASSERT(x >= 0 && x < IMAGE_WIDTH && y >= 0 && y < IMAGE_HEIGHT);
        previewImage[(y*IMAGE_WIDTH+x)*4 + rgb] = value;
    }

    inline void writePreviewPixelThick(std::vector<uint8_t>& previewImage, int x, int y, int rgb, uint8_t value)
    {
        ASSERT(x >= 1 && x < IMAGE_WIDTH-1 && y >= 1 && y < IMAGE_HEIGHT-1);
        previewImage[((y+1)*IMAGE_WIDTH+x+1)*4 + rgb] = value;
        previewImage[((y+1)*IMAGE_WIDTH+x-1)*4 + rgb] = value;
        previewImage[((y-1)*IMAGE_WIDTH+x+1)*4 + rgb] = value;
        previewImage[((y-1)*IMAGE_WIDTH+x-1)*4 + rgb] = value;
        previewImage[(y*IMAGE_WIDTH+x)*4 + rgb] = value;
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

    Position<int> moveTillBorder(uint8_t* data, const Position<int>& start, int moveX, int colorThresholdDark, std::vector<uint8_t>& previewImage)
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
                writePreviewPixel(previewImage, x, start.y, 0, 255);
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

    std::vector<Position<int>> followLine(uint8_t* data, const Position<int>& firstPoint, const Position<int>& secondPoint, int move, int colorThresholdDark, const std::vector<Position<int>>& circleOffsets, std::vector<uint8_t>& previewImage)
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
            writePreviewPixel(previewImage, line.back().x, line.back().y, 1, 255);
#endif
            step += 1;
        }

        return line;
    }

    void findLines(std::vector<uint8_t>& previewImage, uint8_t* data, size_t size)
    {
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


        Position<int> leftFirst = moveTillBorder(data, start, -1, colorThresholdDark, previewImage);
        Position<int> leftSecond = moveTillBorder(data, leftFirst+Position<int>(20, -3), -1, colorThresholdDark, previewImage);
        Position<int> rightFrist = moveTillBorder(data, start, 1, colorThresholdDark, previewImage);
        Position<int> rightSecond = moveTillBorder(data, rightFrist+Position<int>(-20, -3), 1, colorThresholdDark, previewImage);

        auto lineLeft = followLine(data, leftFirst, leftSecond, 4, colorThresholdDark, circleOffsets, previewImage);
        auto lineRight = followLine(data, rightFrist, rightSecond, 4, colorThresholdDark, circleOffsets, previewImage);


        calculateSteering(previewImage, lineLeft, lineRight);
    }

    void getPerpendicular(const std::vector<Position<float>>& line, std::vector<Position<float>>* linePerpendicular, bool flipDirection)
    {
        ASSERT(line.size() == linePerpendicular->size());
        std::vector<Position<float>>& perpendicular = *linePerpendicular;

        int count = 3;
        for(int i = count; i < line.size()-count; ++i)
        {
            Position<float> pointA(0.f, 0.f);
            Position<float> pointB(0.f, 0.f);
            
            for(int l = 1; l <= count; l++)
            {
                pointA.x += line[i-count].x;
                pointB.x += line[i+count].x;

                pointA.y += line[i-count].y;
                pointB.y += line[i+count].y;
            }

            pointA = pointA / (float)count;
            pointB = pointB / (float)count;

            if(flipDirection)
            {
                perpendicular[i].x = (pointB.y - pointA.y);
                perpendicular[i].y = -(pointB.x - pointA.x);
            }
            else
            {
                perpendicular[i].x = -(pointB.y - pointA.y);
                perpendicular[i].y = (pointB.x - pointA.x);
            }

            perpendicular[i] = perpendicular[i] / std::sqrt(perpendicular[i].x * perpendicular[i].x + perpendicular[i].y * perpendicular[i].y);
        }

        for(int i = 0; i < count; ++i)
            perpendicular[i] = perpendicular[count];

        for(int i = perpendicular.size()-count; i < perpendicular.size(); ++i)
            perpendicular[i] = perpendicular[count];
    }

    void calculateSteering(std::vector<uint8_t>& previewImage, const std::vector<Position<int>>& pointsOnLineLeft, const std::vector<Position<int>>& pointsOnLineRight)
    {
        float rotation = 0.f;
        float throtle = 0.f;
        int minLineSteps = 30;
        int centerLineIndex = 20;

        if(pointsOnLineLeft.size() < minLineSteps)
            return;
        if(pointsOnLineRight.size() < minLineSteps)
            return;

        std::vector<Position<float>> lineLeft;
        std::vector<Position<float>> lineRight;
        std::vector<Position<float>> lineCenter;
        lineCenter.reserve(centerLineIndex+1);

        lineLeft.reserve(pointsOnLineLeft.size());
        for(auto& p : pointsOnLineLeft)
            lineLeft.push_back(p);

        lineRight.reserve(pointsOnLineRight.size());
        for(auto& p : pointsOnLineRight)
            lineRight.push_back(p);


        Position<float> center((lineLeft[0] + lineRight[0]) * 0.5f);
        Position<float> direction(0.f, -1.f);
        Average<Position<float>> avDir(3);
        avDir.addSample(Position<float>(0.f, -1.f));
        float rad = 0.6f;
        float moveDistance = 3.f;

#if DEFINED(DEBUG)
        while (true)
#else
        for(int i = 0; i <= centerLineIndex; ++i)
#endif
        {
            lineCenter.push_back(center);

            Position<float> dir = avDir.getAverage();
            dir = dir / std::sqrt(dir.x*dir.x+dir.y*dir.y);
            float directionRad = std::atan2(dir.y, dir.x);

            center = center + dir * moveDistance;

            int indexLeft = 0;
            float spaceLeft = 1e10;
            for(int l = 0; l < lineLeft.size()-4; ++l)
            {
                float sx = lineLeft[l].x - center.x;
                float sy = lineLeft[l].y - center.y;

                float rotation = std::atan2(sy, sx);
                float mod = std::fmod(rotation - (directionRad-1.57079637f), (float)(M_PI*2));
                if(l%10==0)
                    int aaa=0;
                if(std::abs(mod) > rad && std::abs(mod-(float)(M_PI*2)) > rad)
                    continue;

                float space = sx * sx + sy * sy;
                if(space < spaceLeft)
                {
                    spaceLeft = space;
                    indexLeft = l;
                }
            }
            int indexRight = 0;
            float spaceRight = 1e10;
            for(int l = 0; l < lineRight.size()-4; ++l)
            {
                float sx = lineRight[l].x - center.x;
                float sy = lineRight[l].y - center.y;

                float rotation = std::atan2(sy, sx);
                float mod = std::fmod(rotation - (directionRad+1.57079637f), (float)(M_PI*2));
                if(std::abs(mod) > rad && std::abs(mod-(float)(M_PI*2)) > rad)
                    continue;

                float space = sx * sx + sy * sy;
                if(space < spaceRight)
                {
                    spaceRight = space;
                    indexRight = l;
                }
            }

            if(spaceLeft == 1e10 || spaceRight == 1e10)
                break;

            Position<float> direction = (lineLeft[indexLeft+4] + lineRight[indexRight+4]) * 0.5f - center;
            avDir.addSample(direction);

            float spaceDiff = (std::sqrt(spaceLeft) - std::sqrt(spaceRight)) * 0.5f;
            center.x += dir.y * spaceDiff;
            center.y += -dir.x * spaceDiff;
        }


#if DEFINED(DEBUG)
        for(int i = 0; i < lineCenter.size(); ++i)
        {
            const auto& p = lineCenter[i];
            if(p.x >= 1 && p.x < IMAGE_WIDTH-1 && p.y >= 1 && p.y < IMAGE_HEIGHT-1)
                writePreviewPixelThick(previewImage, p.x, p.y, 0, 0);
        }
#endif
            

        ASSERT(lineCenter.size() > centerLineIndex)
        if(lineCenter.size() <= centerLineIndex)
            return;

        Position<float> pointCar(IMAGE_WIDTH / 2, IMAGE_HEIGHT + IMAGE_HEIGHT / 4);
        Position<float>& pointTarget = lineCenter[centerLineIndex];

        rotation = std::atan2(-(pointTarget.y - pointCar.y), pointTarget.x - pointCar.x);
        rotation = (M_PI_2 - rotation) / M_PI_2;

#if !DEFINED(PC_MODE)
        Controller::getInstance()->setThrotle(throtle);
        Controller::getInstance()->setRotation(rotation);
#endif
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
        uint8_t* previewImage = imageData->data();

        int indexInput = 0;
        for(int i = 0; i < imageSize; ++i)
        {
            previewImage[i] = (uint8_t)input[indexInput];
            indexInput += 3;
        }

        delete[] input;
    }
}
