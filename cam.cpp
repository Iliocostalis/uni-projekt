#include <cam.h>
#include <iostream>

void print(int value)
{
    std::cout << value << std::endl;
}

void Cam::init()
{
    libcamera::CameraManager cameraManager;

	int ret = cameraManager.start();
	print(ret);

    int countCam = cameraManager.cameras().size();
    std::cout << "count cam: " << countCam << std::endl;

    std::string cameraId = cameraManager.cameras()[0]->id();

    camera = cameraManager.get(cameraId);
    if(!camera)
        throw std::exception();

    ret = camera->acquire();

    if(ret)
        throw std::exception();

    config = camera->generateConfiguration({libcamera::StreamRole::Viewfinder});

    libcamera::Size size(640, 480);
    libcamera::Transform transform = libcamera::Transform::Identity;

    config->at(0).pixelFormat = libcamera::formats::RGB888;
    config->at(0).size = size;
    config->at(0).bufferCount = 4;
    config->transform = transform;

    switch(config->validate())
    {
        case libcamera::CameraConfiguration::Valid:
            std::cout << "Valid" << std::endl;
            break;

        case libcamera::CameraConfiguration::Adjusted:
            std::cout << "Adjusted" << std::endl;
            break;

        case libcamera::CameraConfiguration::Invalid:
            std::cout << "Invalid" << std::endl;
            break;
    }
}

void Cam::start()
{
    int ret = camera->configure(config.get());

    if(ret)
        throw std::exception();

	camera->requestCompleted.connect(this, &requestQueue);

}

void Cam::stop()
{
    camera->release();
}