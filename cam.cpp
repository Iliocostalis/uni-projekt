#include <cam.h>
#include <iostream>
#include <memory>
#include <iomanip>
#include <thread>
#include <chrono>
#include <sys/mman.h>
#include <fstream>
#include "imageProcessing.h"
#include <atomic>
#include <pthread.h>

//#include "event_loop.h"

//EventLoop loop;
Cam* cam;
std::map<libcamera::FrameBuffer*, std::vector<libcamera::Span<uint8_t>>> mapped_buffers;
std::atomic_bool cameraRunning(false);
std::atomic_bool threadRunning(false);

#define ADD_RAW_STREAM 0
#define DEFINED(v) (v == 1)

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

auto old = std::chrono::high_resolution_clock::now();
uint64_t timeSum = 0;

void* threadFunc(void* arg)
{
    while(threadRunning.load(std::memory_order_acquire))
    {
		auto now = std::chrono::high_resolution_clock::now();
		auto milliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(now - old);
    	old = now;

		timeSum = timeSum + milliseconds.count();

		if(timeSum > 1000)
		{
			timeSum = 0;
			std::cout << "thread running" << std::endl;
			std::cout << threadRunning.load(std::memory_order_acquire) << std::endl;
		}

        Cam* cam = Cam::getInstance();
        if(cam->queue.size() > 0)
        {
			std::cout << "thread working" << std::endl;
            cam->queue.front()();
            cam->queue.pop_front();
        }

    	std::this_thread::sleep_for(std::chrono::milliseconds(2));
    }
}

Cam* Cam::getInstance()
{
    return cam;
}

void Cam::processRequest(libcamera::Request *request)
{
	std::cout << std::endl
		  << "Request completed: " << request->toString() << std::endl;

	/*
	 * When a request has completed, it is populated with a metadata control
	 * list that allows an application to determine various properties of
	 * the completed request. This can include the timestamp of the Sensor
	 * capture, or its gain and exposure values, or properties from the IPA
	 * such as the state of the 3A algorithms.
	 *
	 * ControlValue types have a toString, so to examine each request, print
	 * all the metadata for inspection. A custom application can parse each
	 * of these items and process them according to its needs.
	 */
	//const libcamera::ControlList &requestMetadata = request->metadata();
	//for (const auto &ctrl : requestMetadata) {
	//	const libcamera::ControlId *id = libcamera::controls::controls.at(ctrl.first);
	//	const libcamera::ControlValue &value = ctrl.second;

	//	std::cout << "\t" << id->name() << " = " << value.toString()
	//		  << std::endl;
	//}

	/*
	 * Each buffer has its own FrameMetadata to describe its state, or the
	 * usage of each buffer. While in our simple capture we only provide one
	 * buffer per request, a request can have a buffer for each stream that
	 * is established when configuring the camera.
	 *
	 * This allows a viewfinder and a still image to be processed at the
	 * same time, or to allow obtaining the RAW capture buffer from the
	 * sensor along with the image as processed by the ISP.
	 */
	const libcamera::Request::BufferMap &buffers = request->buffers();
	for (auto bufferPair : buffers) {
		libcamera::FrameBuffer* buffer = bufferPair.second;
		//const libcamera::FrameMetadata &metadata = buffer->metadata();

		/* Print some information about the buffer which has completed. */
		//std::cout << " seq: " << std::setw(6) << std::setfill('0') << metadata.sequence
		//	  << " timestamp: " << metadata.timestamp
		//	  << " bytesused: ";

		//unsigned int nplane = 0;
		//for (const libcamera::FrameMetadata::Plane &plane : metadata.planes())
		//{
		//	std::cout << plane.bytesused;
		//	if (++nplane < metadata.planes().size())
		//		std::cout << "/";
		//}
//
		//std::cout << std::endl;
        
		/*
		 * Image data can be accessed here, but the FrameBuffer
		 * must be mapped by the application
		 */

        //int co = 0;

        ////auto myfile = std::ofstream("file.ppm", std::ios::out | std::ios::binary);
        ////myfile << "P6" << "\n" << 640 << " " << 480 << "\n" << 255 << "\n";

        libcamera::Span span = Mmap(buffer)[0];
        ImageProcessing::process(span.data(), span.size_bytes());
        
        ////myfile.write((char*)span.data(), span.size_bytes());
        //for(auto it = span.begin(); it != span.end(); ++it)
        //{
            //co++;
            //if(co > 100)
            //    break;
            //std::cout << (int)*it << "/";
        //}
        //std::cout << std::endl;
        ////myfile.close();
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
    cam = this;

    cameraManager = std::make_shared<libcamera::CameraManager>();
    //libcamera::CameraManager cameraManager;

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
	//StreamRole::VideoRecording

#if DEFINED(ADD_RAW_STREAM)
    config = camera->generateConfiguration({libcamera::StreamRole::Viewfinder, libcamera::StreamRole::Raw});
#else
	config = camera->generateConfiguration({libcamera::StreamRole::Viewfinder});
#endif

	libcamera::Size size(1280, 960);
    //libcamera::Size size(640, 480);
    libcamera::Size sizePref(size);

	/*if (camera->properties().contains(libcamera::properties::PixelArrayActiveAreas))
	{
		// The idea here is that most sensors will have a 2x2 binned mode that
		// we can pick up. If it doesn't, well, you can always specify the size
		// you want exactly with the viewfinder_width/height options_->
		size = camera->properties().get(libcamera::properties::PixelArrayActiveAreas)[0].size() / 2;
		// If width and height were given, we might be switching to capture
		// afterwards - so try to match the field of view.
		//if (options_->width && options_->height)
		size = size.boundedToAspectRatio(sizePref);
		size.alignDownTo(2, 2); // YUV420 will want to be even
		//LOG(2, "Viewfinder size chosen is " << size.toString());

		std::cout << "2x3 yes" << std::endl;
	}*/

	std::cout << "size: " << size.width << " " << size.height << std::endl;

    libcamera::Transform transform = libcamera::Transform::Identity;

    //config->at(0).pixelFormat = libcamera::formats::RGB888;
    //config->at(0).pixelFormat = libcamera::formats::BGR888;
    // err config->at(0).pixelFormat = libcamera::formats::SGBRG10;
    config->at(0).pixelFormat = libcamera::formats::YUV420;
    config->at(0).size = size;
    config->at(0).bufferCount = 6;
	//config->at(0).colorSpace = libcamera::ColorSpace::Rec709;
	//config->at(0).colorSpace = libcamera::ColorSpace::Smpte170m;
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
	std::vector<std::unique_ptr<libcamera::Request>> requests;
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
		controls.set(libcamera::controls::Brightness, 0.5);

		requests.push_back(std::move(request));
	}


    camera->requestCompleted.connect(requestComplete);


	libcamera::ControlList controls;
	int64_t framerate = 90;
	int64_t frame_time = 1000000 / framerate; // in us
	controls.set(libcamera::controls::FrameDurationLimits, { frame_time, frame_time });

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



	std::this_thread::sleep_for(std::chrono::seconds(3));
    cameraRunning.store(false, std::memory_order_release);
    std::this_thread::sleep_for(std::chrono::seconds(1));
    threadRunning.store(false, std::memory_order_release);

	std::cout << "Thread exiting" << std::endl;

    int returnVal;
    int* returnValP = &returnVal;
    pthread_join(thread, (void**)&returnValP);

	std::cout << "Thread terminated" << std::endl;


	//libcamera::StreamConfiguration &streamConfig = config->at(0);
	//libcamera::Stream *stream = streamConfig.stream();

	camera->stop();
	allocator->free(stream);
	allocator.reset();
	camera->release();
	camera.reset();
	cameraManager->stop();
	cameraManager.reset();
}

void Cam::stop()
{
    cameraRunning = false;
    std::this_thread::sleep_for(std::chrono::seconds(1));
    threadRunning = false;

    int returnVal;
    int* returnValP = &returnVal;
    pthread_join(thread, (void**)&returnValP);


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