#include <DevCam.h>

#include <ImageProcessing.h>
#include <filesystem>
#include <iostream>
#include <chrono>
#include <iostream>
#include <thread>

void DevCam::cameraLoop()
{
	int index = 0;
	while (cameraRunning)
	{
		// index = 81;
#if DEFINED(CAMERA_LOG)
		std::cout << "load image: " << imageNames[index] << " index: " << index << std::endl;
#endif
		uint8_t *data = images[index].data();
		int size = images[index].size();
		ImageProcessing::process(data, size);
		index = (index + 1) % images.size();

		std::this_thread::sleep_for(std::chrono::milliseconds(200));
	}

	return;
}

DevCam::DevCam() : cameraRunning(false)
{
}

DevCam *DevCam::getInstance()
{
	static DevCam DevCam;
	return &DevCam;
}

bool stringCompare(const std::string& s1, const std::string& s2)
{
	if(s1.size() == s2.size())
		return std::lexicographical_compare(s1.begin(), s1.end(), s2.begin(), s2.end());
	return s1.size() < s2.size();
}

void DevCam::init()
{
	int count = 0;
	std::wstring imageFolder = std::filesystem::current_path().wstring() + L"/images";
	for (const auto &entry : std::filesystem::directory_iterator(imageFolder))
	{
		if (entry.is_directory())
			continue;
		++count;
	}

	images.resize(count);
	imageNames.reserve(count);
	for (const auto &entry : std::filesystem::directory_iterator(imageFolder))
	{
		if (entry.is_directory())
			continue;
		imageNames.push_back(entry.path().string());
	}

	std::sort(imageNames.begin(), imageNames.end(), stringCompare);

	for (int i = 0; i < imageNames.size(); ++i)
	{
		ImageProcessing::readImageFromFolder(imageNames[i], &images[i]);
	}

	if(imageNames.size() > 0)
		initSuccessful = true;
}

bool DevCam::wasInitSuccessful()
{
	return initSuccessful;
}

void DevCam::start()
{
	cameraRunning = true;
	camThread = std::thread(std::bind(&DevCam::cameraLoop, this));
}

void DevCam::stop()
{
	cameraRunning = false;
	if (camThread.joinable())
		camThread.join();
}