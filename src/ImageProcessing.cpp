#include <ImageProcessing.h>
#include <iostream>
#include <fstream>
#include <string>
#include <chrono>
#define NOMINMAX
#define _USE_MATH_DEFINES
#include <math.h>
#include <cmath>
#include <Debugging.h>
#include <Controller.h>

namespace ImageProcessing
{
    int imageNameCount = 0;
    std::atomic_bool saveVideoBool(false);
    auto timeLastImageSaved = std::chrono::high_resolution_clock::now();
    auto timeLast = std::chrono::high_resolution_clock::now();

    Average<std::chrono::microseconds> averageTime(5);
    std::chrono::microseconds lasts[5];

    Position<int> startPosition;
    int averageStreetColor;
    int streetColorDark;

    void copyCurrentImageToWindow(uint8_t* data)
    {
        uint8_t* previewImage = window->getImagePointer(window->getNextImageIndex());
        for(int i = 0; i < IMAGE_WIDTH * IMAGE_HEIGHT; ++i)
        {
            previewImage[i * 4 + 0] = data[i];
            previewImage[i * 4 + 1] = data[i];
            previewImage[i * 4 + 2] = data[i];
            //previewImage[i * 4 + 3] = 255; // ignore alpha -> better performance
        }
    }

    

    inline void writePreviewPixel(uint8_t* previewImage, int x, int y, int rgb, uint8_t value)
    {
        if(x >= 0 && x < IMAGE_WIDTH && y >= 0 && y < IMAGE_HEIGHT)
            previewImage[(y*IMAGE_WIDTH+x)*4 + rgb] = value;
    }

    inline void writePreviewPixelThick(uint8_t* previewImage, int x, int y, int rgb, uint8_t value)
    {
        if(x >= 1 && x < IMAGE_WIDTH-1 && y >= 1 && y < IMAGE_HEIGHT-1)
        {
            previewImage[((y+1)*IMAGE_WIDTH+x+1)*4 + rgb] = value;
            previewImage[((y+1)*IMAGE_WIDTH+x-1)*4 + rgb] = value;
            previewImage[((y-1)*IMAGE_WIDTH+x+1)*4 + rgb] = value;
            previewImage[((y-1)*IMAGE_WIDTH+x-1)*4 + rgb] = value;
            previewImage[(y*IMAGE_WIDTH+x)*4 + rgb] = value;
        }
    }

    Position<int> moveTillBorder(uint8_t* image, const Position<int>& start, int moveX, int streetColorDark, uint8_t* previewImage)
    {
        ASSERT(moveX == 1 || moveX == -1)

        Position<int> pos(start);

        int sum = 0;
        int min = 255;
        int max = 0;
        for(int x = start.x; x >= 0 && x < IMAGE_WIDTH; x += moveX)
        {
            int val = (int)image[start.y * IMAGE_WIDTH + x];
            min = std::min(min, val);
            max = std::max(max, val);

            if(val < streetColorDark)
            {
                sum += 1;
            }
            else
            {
                if(areLinesVisible)
                {
                    writePreviewPixel(previewImage, x, start.y, 0, 255);
                    writePreviewPixel(previewImage, x, start.y, 1, 0);
                    writePreviewPixel(previewImage, x, start.y, 2, 0);
                }
                sum = std::max(0, sum - 2);
            }

            if(sum >= 3)
            {
                pos.x = x - 3*moveX;
                break;
            }

            pos.x = x;
        }

        return pos;
    }

    Position<float> moveTillBorder(uint8_t* image, const Position<float>& start, const Position<float>& moveDirection, int streetColorDark, int stepCount, int blackCountToStop, bool* found, uint8_t* previewImage)
    {
        Position<float> pos(start);

        *found = true;
        int sum = 0;
        int min = 255;
        int max = 0;
        int count = 0;
        while(pos.x >= 0 && pos.x <= IMAGE_WIDTH && pos.y >= 0 && pos.y <= IMAGE_HEIGHT)
        {
            if(count >= stepCount)
            {
                *found = false;
                break;
            }
            count += 1;

            int val = image[(int)pos.y * IMAGE_WIDTH + (int)pos.x];
            if(val < streetColorDark)
            {
                sum += 1;
            }
            else
            {
                if(areLinesVisible)
                {
                    writePreviewPixel(previewImage, (int)pos.x, (int)pos.y, 0, 0);
                    writePreviewPixel(previewImage, (int)pos.x, (int)pos.y, 1, 255);
                    writePreviewPixel(previewImage, (int)pos.x, (int)pos.y, 2, 0);
                }
                sum = std::max(0, sum - 2);
            }

            if(sum >= blackCountToStop)
            {
                pos = pos - moveDirection * (float)sum;
                break;
            }

            pos = pos + moveDirection;
        }

        return pos;
    }

    void followLine(uint8_t* image, const Position<int>& firstPoint, const Position<int>& secondPoint, int streetColorDark, uint8_t* previewImage, std::vector<Position<int>>* line)
    {
        float moveInside = 30.f;
        float moveSize = 5.f;

        Position<float> pointNext(secondPoint);
        Position<float> point(secondPoint);
        Position<float> pointLastValid(firstPoint);
        Position<float> direction(secondPoint-firstPoint);

        bool isLineLeft = firstPoint.x < IMAGE_WIDTH / 2;

        Average<Position<float>> avDir(5);
        avDir.addSample(direction / std::sqrt(direction.x*direction.x+direction.y*direction.y));
        direction = avDir.getAverage();

        bool gapDetected = false;
        bool stepDetected = false;
        bool stepCounterToHigh = false;
        int stepCounter = 0;
        int cutCount = 0;
        int maxCuts = 5;
        int index = 0;
        while(true)
        {
#if !DEFINED(DEBUG)
            if(index > 70 || point.y < IMAGE_HEIGHT / 2)
                return;
#else
            if(index > 200)
                return;
#endif

            index += 1;
            line->push_back(point);

            float moveInsideScaled = moveInside / (1.f + 3.f * (1.0f - point.y / IMAGE_HEIGHT));
            float moveSizeScaled = moveSize / (1.f + 1.f * (1.0f - point.y / IMAGE_HEIGHT));
            int findGapScaled = (int)(8.f / (1.f + 2.f * (1.0f - point.y / IMAGE_HEIGHT)));


            Position<float> directionNew = pointNext - pointLastValid;
            directionNew = directionNew / std::sqrt(directionNew.x*directionNew.x+directionNew.y*directionNew.y);

            Position<float> dirDiffP = directionNew - direction;
            float dirDiff = std::sqrt(dirDiffP.x * dirDiffP.x + dirDiffP.y * dirDiffP.y);

            if(gapDetected)
            {
                point = pointNext;
                pointLastValid = point;
                avDir.addSample(directionNew);
            }
            else if(dirDiff > 0.7f && !stepDetected)
            {
                if(stepCounterToHigh)
                    return;

                if(stepCounter > 0)
                {
                    stepCounterToHigh = true;
                    stepCounter = 6;
                }
                else
                {
                    stepCounterToHigh = false;
                    stepCounter = 4;
                }

                stepDetected = true;
                point = point + direction * moveSizeScaled;
            }
            else
            {
                stepCounter -= 1;
                stepDetected = false;
                point = pointNext;
                pointLastValid = point;
                avDir.addSample(directionNew);
            }
            direction = avDir.getAverage();


            Position<float> directionInside;
            Position<float> directionOutside;
            if(isLineLeft)
            {
                directionInside.x = -direction.y;
                directionInside.y = direction.x;

                directionOutside.x = direction.y;
                directionOutside.y = -direction.x;
            }
            else
            {
                directionInside.x = direction.y;
                directionInside.y = -direction.x;

                directionOutside.x = -direction.y;
                directionOutside.y = direction.x;
            }

            pointNext = point + direction * moveSizeScaled + directionInside * moveInsideScaled;

            moveTillBorder(image, pointNext + direction * (float)-findGapScaled, direction, streetColorDark, findGapScaled*2, 1, &gapDetected, previewImage);
            if(!gapDetected)
                moveTillBorder(image, pointNext + direction * (float)-findGapScaled + directionOutside * (moveInsideScaled * 0.2f), direction, streetColorDark, findGapScaled*2, 1, &gapDetected, previewImage);
            if(gapDetected)
            {
                if(cutCount > maxCuts)
                    return;

                cutCount += 1;
                pointNext = point + direction * moveSizeScaled;
                continue;
            }
            else
                cutCount = 0;

            bool borderFound;
            pointNext = moveTillBorder(image, pointNext, directionOutside, streetColorDark, moveInsideScaled*3.f, 2, &borderFound, previewImage);

            if(!borderFound)
                return;

        }
    }

    int getAverageStreetColor(const Position<int>& position, uint8_t* image)
    {
        int averageColorA = 0;
        for(int y = -4; y <= 4; ++y)
        {
            for(int x = -4; x <= 4; ++x)
            {
                int ix = position.x + x;
                int iy = position.y + y;
                averageColorA += (int)image[iy * IMAGE_WIDTH + ix];
            }
        }

        int averageColorB = 0;
        for(int y = -4; y <= 4; ++y)
        {
            for(int x = -4; x <= 4; ++x)
            {
                int ix = position.x + x;
                int iy = position.y + y - IMAGE_HEIGHT / 8;
                averageColorB += (int)image[iy * IMAGE_WIDTH + ix];
            }
        }

        return std::max(averageColorA, averageColorB) / 81;
    }

    void findLines(uint8_t* previewImage, uint8_t* image, std::vector<Position<int>>* lineLeft, std::vector<Position<int>>* lineRight)
    {
        Position<int> leftFirst = moveTillBorder(image, startPosition, -1, streetColorDark, previewImage);
        Position<int> rightFrist = moveTillBorder(image, startPosition, 1, streetColorDark, previewImage);
        // redo a bit higher in order find the direction of the border
        Position<int> leftSecond = moveTillBorder(image, leftFirst+Position<int>(20, -3), -1, streetColorDark, previewImage);
        Position<int> rightSecond = moveTillBorder(image, rightFrist+Position<int>(-20, -3), 1, streetColorDark, previewImage);

        
        followLine(image, leftFirst, leftSecond, streetColorDark, previewImage, lineLeft);
        followLine(image, rightFrist, rightSecond, streetColorDark, previewImage, lineRight);

        // if the found border is to short try it again a bit higher
        if(lineLeft->size() < 8)
        {
            Position<int> leftFirst = moveTillBorder(image, startPosition+Position<int>(0, -20), -1, streetColorDark, previewImage);
            Position<int> leftSecond = moveTillBorder(image, leftFirst+Position<int>(20, -23), -1, streetColorDark, previewImage);
            lineLeft->clear();
            followLine(image, leftFirst, leftSecond, streetColorDark, previewImage, lineLeft);
        }

        if(lineRight->size() < 8)
        {
            Position<int> leftFirst = moveTillBorder(image, startPosition+Position<int>(0, -20), -1, streetColorDark, previewImage);
            Position<int> rightSecond = moveTillBorder(image, leftFirst+Position<int>(20, -23), -1, streetColorDark, previewImage);
            lineRight->clear();
            followLine(image, leftFirst, rightSecond, streetColorDark, previewImage, lineRight);
        }

        if(areLinesVisible)
        {
            for(int i = 0; i < lineLeft->size(); ++i)
            {
                const auto& p = (*lineLeft)[i];
                if(p.x >= 1 && p.x < IMAGE_WIDTH-1 && p.y >= 1 && p.y < IMAGE_HEIGHT-1)
                {
                    writePreviewPixelThick(previewImage, p.x, p.y, 0, 0);
                    writePreviewPixelThick(previewImage, p.x, p.y, 1, 0);
                    writePreviewPixelThick(previewImage, p.x, p.y, 2, 255);
                }
            }
            for(int i = 0; i < lineRight->size(); ++i)
            {
                const auto& p = (*lineRight)[i];
                if(p.x >= 1 && p.x < IMAGE_WIDTH-1 && p.y >= 1 && p.y < IMAGE_HEIGHT-1)
                {
                    writePreviewPixelThick(previewImage, p.x, p.y, 0, 0);
                    writePreviewPixelThick(previewImage, p.x, p.y, 1, 0);
                    writePreviewPixelThick(previewImage, p.x, p.y, 2, 255);
                }
            }
        }
    }

    float getPercentageDarkPixels(uint8_t* image, uint8_t* previewImage, int streetColorDark) {
        int rows = 10;
        int startHeight = (int)((float)IMAGE_HEIGHT * 0.8f);
        int endHeight = IMAGE_HEIGHT - 20;   
        int width = (int)((float)IMAGE_WIDTH * 0.3f);
        int space = width / rows;  
        int startX = (IMAGE_WIDTH - width) / 2;   
        int countDarkPixel = 0;  

        for(int r = 0; r < rows; ++r)    {   
            int currentX = startX + space * r;   
            for(int y = startHeight; y < endHeight; ++y) {        
                if(image[y * IMAGE_WIDTH + currentX] <= streetColorDark) {
                    countDarkPixel += 1;                    
                }
                if(isPreviewVisible)
                {
                    writePreviewPixel(previewImage, currentX, y, 0, 0);
                    writePreviewPixel(previewImage, currentX, y, 1, 0);
                    writePreviewPixel(previewImage, currentX, y, 2, 255);
                }
            } 
        }  
        return (float)countDarkPixel / (float)(rows * endHeight - startHeight);
    }

    void calculateSteering(uint8_t* previewImage, const std::vector<Position<int>>& pointsOnLineLeft, const std::vector<Position<int>>& pointsOnLineRight)
    {
        float rotation = 0.f;
        int minLineSteps = 6;

        if(pointsOnLineLeft.size() < minLineSteps)
            return;
        if(pointsOnLineRight.size() < minLineSteps)
            return;

        std::vector<Position<float>> lineLeft;
        std::vector<Position<float>> lineRight;
        std::vector<Position<float>> lineCenter;
        lineCenter.reserve(100);

        lineLeft.reserve(pointsOnLineLeft.size());
        for(auto& p : pointsOnLineLeft)
            lineLeft.push_back(p);

        lineRight.reserve(pointsOnLineRight.size());
        for(auto& p : pointsOnLineRight)
            lineRight.push_back(p);


        Position<float> center((lineLeft[0] + lineRight[0]) * 0.5f);
        Position<float> direction(0.f, -1.f);
        Average<Position<float>> avDir(5);
        avDir.addSample(Position<float>(0.f, -1.f));
        float rad = 0.6f;
        float moveDistance = 3.f;

        while (true)
        {
            lineCenter.push_back(center);
            if(center.y <= (IMAGE_HEIGHT * 4) / 6)
                break;

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
            avDir.addSample(direction / std::sqrt(direction.x*direction.x+direction.y*direction.y));

            float spaceDiff = (std::sqrt(spaceLeft) - std::sqrt(spaceRight)) * 0.5f;
            center.x += dir.y * spaceDiff;
            center.y += -dir.x * spaceDiff;
        }


        if(areLinesVisible)
        {
            for(int i = 0; i < lineCenter.size(); ++i)
            {
                const auto& p = lineCenter[i];
                if(p.x >= 1 && p.x < IMAGE_WIDTH-1 && p.y >= 1 && p.y < IMAGE_HEIGHT-1)
                {
                    writePreviewPixelThick(previewImage, p.x, p.y, 2, 0);
                    writePreviewPixelThick(previewImage, p.x, p.y, 1, 0);
                    writePreviewPixelThick(previewImage, p.x, p.y, 0, 255);
                }
            }
        }
            

        Position<float> pointTarget = lineCenter.back();
        for(const auto& cl : lineCenter)
        {
            if(cl.y <= (IMAGE_HEIGHT * 4) / 6)
            {
                pointTarget = cl;
                break;
            }
        }

        Position<float> pointCar(IMAGE_WIDTH / 2, IMAGE_HEIGHT + IMAGE_HEIGHT / 4);

        rotation = std::atan2(-(pointTarget.y - pointCar.y), pointTarget.x - pointCar.x);
        rotation = (M_PI_2 - rotation) / M_PI_2 * 5.f;
        float rotationNew = 0;
        if(rotation <= 0)
            rotationNew = -rotation*rotation;
        else
            rotationNew = rotation*rotation;

        if(rotation < 0.1 && rotation > -0.1)
            rotationNew = rotation;

#if !DEFINED(USE_OHMCARSIMULATOR)
        rotationNew -= 0.3f;
#endif

        if(areLinesVisible)
        {
            writePreviewPixelThick(previewImage, pointTarget.x, pointTarget.y, 0, 0);
            writePreviewPixelThick(previewImage, pointTarget.x, pointTarget.y, 2, 0);
            writePreviewPixelThick(previewImage, pointTarget.x, pointTarget.y, 1, 255);
        }
        
        Controller::getInstance()->setRotation(rotationNew);
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

    void process(uint8_t* image, size_t size)
    {
        uint8_t* previewImage = nullptr;
        if(isPreviewVisible)
        {
            previewImage = window->getImagePointer(window->getNextImageIndex());
            copyCurrentImageToWindow(image);
        }

        startPosition.x = IMAGE_WIDTH / 2;
        startPosition.y = (IMAGE_HEIGHT / 8) * 7;

        averageStreetColor = getAverageStreetColor(startPosition, image);
        streetColorDark = (int)((float)averageStreetColor * 0.8f);

        float percentageDarkPixels = getPercentageDarkPixels(image, previewImage, streetColorDark);

        Controller::getInstance()->updatePercentageDarkPixelsInStartStopLine(percentageDarkPixels);

        std::vector<Position<int>> lineLeft;
        std::vector<Position<int>> lineRight;
        findLines(previewImage, image, &lineLeft, &lineRight);

        calculateSteering(previewImage, lineLeft, lineRight);


        if(isPreviewVisible)
            window->swap();

        auto currentTime = std::chrono::high_resolution_clock::now();

#if DEFINED(CAMERA_LOG)
        auto microseconds = std::chrono::duration_cast<std::chrono::microseconds>(currentTime - timeLast);
        timeLast = currentTime;
        averageTime.addSample(microseconds);
        std::cout << "image count: " << averageTime.getAverage().count() << std::endl;
        std::cout << "fps: " << 1000000.0f / averageTime.getAverage().count() << std::endl;
        std::cout << "size: " << size << std::endl;
#endif
        auto durationLastImageSaved = std::chrono::duration_cast<std::chrono::milliseconds>(currentTime - timeLastImageSaved);

        if(saveVideoBool && durationLastImageSaved.count() > SAVE_VIDEO_FRAME_DELAY_MS)
        {
            timeLastImageSaved = currentTime;
            std::string fileName = std::string("../images/image_") + std::to_string(imageNameCount);
            imageNameCount += 1;
            if(imageNameCount > 500)
                stopVideo();

            saveImageToFolder(fileName, image);
        }
    }
}
