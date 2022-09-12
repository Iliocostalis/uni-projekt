#include <Cam.h>

#if DEFINED(PC_MODE)
#include <ImageProcessing.h>
#include <filesystem>
#include <iostream>
#include <pthread.h>
#include <chrono>
#include <iostream>
#include <thread>

#else

#include <iostream>
#include <memory>
#include <iomanip>
#include <thread>
#include <chrono>
#include <sys/mman.h>
#include <fstream>
#include <ImageProcessing.h>
#include <atomic>
#include <pthread.h>

#endif

#if !DEFINED(PC_MODE)

void print(int value)
{
    std::cout << value << std::endl;
}

std::vector<libcamera::Span<uint8_t>> Mmap(libcamera::FrameBuffer *buffer)
{
	auto item = mapped_buffers.find(buffer);
	if (item == mapped_buffers.end())
		return {};
	return item->second;
}

void requestComplete(libcamera::Request *request)
{
	if (request->status() == libcamera::Request::RequestCancelled)
		return;

    Cam::getInstance()->queue.push_back(std::bind(&Cam::processRequest, Cam::getInstance(), request));
}

void* threadFunc(void* arg)
{
    while(threadRunning.load(std::memory_order_acquire))
    {
        Cam* cam = Cam::getInstance();
	    
        if(cam->queue.size() > 0)
        {
	    //std::cout << "queue size: " << cam->queue.size() << std::endl;
            cam->queue.front()();
            cam->queue.pop_front();
        }

    	std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
	return (void*)nullptr;
}

Cam::Cam() : threadRunning(false), cameraRunning(false)
{}

Cam* Cam::getInstance()
{
	static Cam cam;
    return &cam;
}

void Cam::processRequest(libcamera::Request *request)
{
#if DEFINED(CAMERA_LOG)
	std::cout << std::endl
		  << "Request completed: " << request->toString() << std::endl;
#endif

	const libcamera::Request::BufferMap &buffers = request->buffers();
	for (auto bufferPair : buffers) 
	{
		libcamera::FrameBuffer* buffer = bufferPair.second;

        libcamera::Span<uint8_t> span = Mmap(buffer)[0];
	if(queue.size() < 3)
	    ImageProcessing::process(span.data(), span.size_bytes());
	}

	/* Re-queue the Request to the camera. */
    if(cameraRunning.load(std::memory_order_acquire))
    {
        request->reuse(libcamera::Request::ReuseBuffers);
	    camera->queueRequest(request);
    }
}

void Cam::init()
{
    cameraManager = std::make_shared<libcamera::CameraManager>();

	int ret = cameraManager->start();
	print(ret);

    int countCam = cameraManager->cameras().size();
    std::cout << "count cam: " << countCam << std::endl;

    std::string cameraId = cameraManager->cameras()[0]->id();

    camera = cameraManager->get(cameraId);
    if(!camera)
        throw std::exception();

    ret = camera->acquire();

    if(ret)
        throw std::exception();

#if DEFINED(ADD_RAW_STREAM)
    config = camera->generateConfiguration({libcamera::StreamRole::Viewfinder, libcamera::StreamRole::Raw});
#else
	config = camera->generateConfiguration({libcamera::StreamRole::Viewfinder});
#endif

	libcamera::Size size(IMAGE_WIDTH, IMAGE_HEIGHT);

    libcamera::Transform transform = libcamera::Transform::Identity;

    //config->at(0).pixelFormat = libcamera::formats::RGB888;
    //config->at(0).pixelFormat = libcamera::formats::BGR888;
    // err config->at(0).pixelFormat = libcamera::formats::SGBRG10;
    //config->at(0).pixelFormat = libcamera::formats::R8;
    config->at(0).pixelFormat = libcamera::formats::YUV420;
    config->at(0).size = size;
    config->at(0).bufferCount = 6;
#if(IMAGE_WIDTH >= 1280)
	config->at(0).colorSpace = libcamera::ColorSpace::Rec709;
#else
	config->at(0).colorSpace = libcamera::ColorSpace::Smpte170m;
#endif
    config->transform = transform;

	// raw
#if DEFINED(ADD_RAW_STREAM)
	config->at(1).pixelFormat = libcamera::formats::SBGGR8;
	config->at(1).size = size;
    config->at(1).bufferCount = 6;
#endif

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
    libcamera::StreamConfiguration &streamConfig = config->at(0);

    allocator = std::make_unique<libcamera::FrameBufferAllocator>(camera);

	for (libcamera::StreamConfiguration &cfg : *config) {
		int ret = allocator->allocate(cfg.stream());
		if (ret < 0) {
			std::cerr << "Can't allocate buffers" << std::endl;
			return;
		}

		size_t allocated = allocator->buffers(cfg.stream()).size();
		std::cout << "Allocated " << allocated << " buffers for stream" << std::endl;

        for (const std::unique_ptr<libcamera::FrameBuffer> &buffer : allocator->buffers(cfg.stream()))
		{
			// "Single plane" buffers appear as multi-plane here, but we can spot them because then
			// planes all share the same fd. We accumulate them so as to mmap the buffer only once.
			size_t buffer_size = 0;
			for (unsigned i = 0; i < buffer->planes().size(); i++)
			{
				const libcamera::FrameBuffer::Plane &plane = buffer->planes()[i];
				buffer_size += plane.length;
				if (i == buffer->planes().size() - 1 || plane.fd.get() != buffer->planes()[i + 1].fd.get())
				{
					void *memory = mmap(NULL, buffer_size, PROT_READ | PROT_WRITE, MAP_SHARED, plane.fd.get(), 0);
					mapped_buffers[buffer.get()].push_back(libcamera::Span<uint8_t>(static_cast<uint8_t *>(memory), buffer_size));
					buffer_size = 0;
				}
			}
			//frame_buffers_[stream].push(buffer.get());
		}
	}



    libcamera::Stream *stream = streamConfig.stream();
	const std::vector<std::unique_ptr<libcamera::FrameBuffer>> &buffers = allocator->buffers(stream);
	for (unsigned int i = 0; i < buffers.size(); ++i) {
		std::unique_ptr<libcamera::Request> request = camera->createRequest();
		if (!request)
		{
			std::cerr << "Can't create request" << std::endl;
			return;
		}

		const std::unique_ptr<libcamera::FrameBuffer> &buffer = buffers[i];
		int ret = request->addBuffer(stream, buffer.get());
		if (ret < 0)
		{
			std::cerr << "Can't set buffer for request"
				  << std::endl;
			return;
		}

		/*
		 * Controls can be added to a request on a per frame basis.
		 */
		libcamera::ControlList &controls = request->controls();
		controls.set(libcamera::controls::Brightness, 0.5f);

		requests.push_back(std::move(request));
	}


    camera->requestCompleted.connect(requestComplete);


	int64_t frame_time = 1000000 / FRAMERATE; // in us
	controls.set(libcamera::controls::FrameDurationLimits, libcamera::Span<const int64_t, 2>({ frame_time, frame_time }));

    cameraRunning.store(true, std::memory_order_release);
    camera->start(&controls);
	for (std::unique_ptr<libcamera::Request> &request : requests)
		camera->queueRequest(request.get());



    threadRunning = true;
    int threadRet;
    threadRet = pthread_create(&thread, NULL, &threadFunc, NULL);
    if(threadRet != 0)
    {
        std::cout << "Thread creation error" << std::endl;
    }
	std::cout << "Thread created" << std::endl;
}

void Cam::stop()
{
    cameraRunning.store(false, std::memory_order_release);
    //std::this_thread::sleep_for(std::chrono::seconds(1));
    threadRunning.store(false, std::memory_order_release);

	std::cout << "Thread exiting" << std::endl;

    void* returnValue;
    pthread_join(thread, &returnValue);

	std::cout << "Thread terminated" << std::endl;

	libcamera::StreamConfiguration &streamConfig = config->at(0);
	libcamera::Stream *stream = streamConfig.stream();

	camera->stop();
	allocator->free(stream);
	allocator.reset();
	camera->release();
	camera.reset();
	cameraManager->stop();
	cameraManager.reset();
}

#else

struct LoopArg
{
	std::vector<std::vector<uint8_t>>* images;
	std::atomic_bool* cameraRunning;
};

Cam::Cam() : cameraRunning(false)
{}

Cam::~Cam()
{
	LoopArg* arg = (LoopArg*)loopArg;
	delete arg;
}

Cam* Cam::getInstance()
{
	static Cam cam;
    return &cam;
}

void Cam::processRequest()
{

}

void Cam::init()
{
	LoopArg* arg = new LoopArg();
	arg->images = &images;
	arg->cameraRunning = &cameraRunning;

	loopArg = (void*)arg;

	int count = 0;
	std::string imageFolder = "../images";
	for (const auto & entry : std::filesystem::directory_iterator(imageFolder))
		++count;

	images.resize(count);
	imageNames.reserve(count);
	for (const auto & entry : std::filesystem::directory_iterator(imageFolder))
	{
        //std::cout << entry.path() << std::endl;
		imageNames.push_back(entry.path());
	}

	for(int i = 0; i < imageNames.size(); ++i)
	{
		ImageProcessing::readImageFromFolder(imageNames[i], &images[i]);
	}
}

void* cameraLoop(void* arg)
{
	LoopArg* loopArg = (LoopArg*)arg;
	std::vector<std::vector<uint8_t>>& images = *(loopArg->images);
	std::atomic_bool& cameraRunning = *(loopArg->cameraRunning);

	int index = 0;
	while(cameraRunning)
	{
		uint8_t* data = images[index].data();
		int size = images[index].size();
		ImageProcessing::process(data, size);
		index = (index + 1) % images.size();

    	std::this_thread::sleep_for(std::chrono::milliseconds(500));
	}

	return (void*) nullptr;
}

void Cam::start()
{
	cameraRunning = true;
	
	int threadRet;
    threadRet = pthread_create(&thread, NULL, &cameraLoop, (void*) loopArg);
}

void Cam::stop()
{
	cameraRunning = false;
	void* returnVal;
	pthread_join(thread, &returnVal);
} 

#endif