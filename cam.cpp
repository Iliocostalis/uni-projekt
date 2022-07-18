#include <cam.h>
#include <iostream>
#include <memory>
#include <iomanip>
#include "event_loop.h"

EventLoop loop;

void print(int value)
{
    std::cout << value << std::endl;
}

void processRequest(Request *request);

void requestComplete(Request *request)
{
	if (request->status() == Request::RequestCancelled)
		return;

	loop.callLater(std::bind(&processRequest, request));
}

void processRequest(Request *request)
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
	const ControlList &requestMetadata = request->metadata();
	for (const auto &ctrl : requestMetadata) {
		const ControlId *id = controls::controls.at(ctrl.first);
		const ControlValue &value = ctrl.second;

		std::cout << "\t" << id->name() << " = " << value.toString()
			  << std::endl;
	}

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
	const Request::BufferMap &buffers = request->buffers();
	for (auto bufferPair : buffers) {
		// (Unused) Stream *stream = bufferPair.first;
		FrameBuffer *buffer = bufferPair.second;
		const FrameMetadata &metadata = buffer->metadata();

		/* Print some information about the buffer which has completed. */
		std::cout << " seq: " << std::setw(6) << std::setfill('0') << metadata.sequence
			  << " timestamp: " << metadata.timestamp
			  << " bytesused: ";

		unsigned int nplane = 0;
		for (const FrameMetadata::Plane &plane : metadata.planes())
		{
			std::cout << plane.bytesused;
			if (++nplane < metadata.planes().size())
				std::cout << "/";
		}

		std::cout << std::endl;

		/*
		 * Image data can be accessed here, but the FrameBuffer
		 * must be mapped by the application
		 */
	}

	/* Re-queue the Request to the camera. */
	request->reuse(Request::ReuseBuffers);
	camera->queueRequest(request);
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

	camera->requestCompleted.connect(this, &Cam::requestComplete);

    FrameBufferAllocator *allocator = new FrameBufferAllocator(camera);

	for (StreamConfiguration &cfg : *config) {
		int ret = allocator->allocate(cfg.stream());
		if (ret < 0) {
			std::cerr << "Can't allocate buffers" << std::endl;
			return EXIT_FAILURE;
		}

		size_t allocated = allocator->buffers(cfg.stream()).size();
		std::cout << "Allocated " << allocated << " buffers for stream" << std::endl;
	}




    Stream *stream = config.stream();
	const std::vector<std::unique_ptr<FrameBuffer>> &buffers = allocator->buffers(stream);
	std::vector<std::unique_ptr<Request>> requests;
	for (unsigned int i = 0; i < buffers.size(); ++i) {
		std::unique_ptr<Request> request = camera->createRequest();
		if (!request)
		{
			std::cerr << "Can't create request" << std::endl;
			return EXIT_FAILURE;
		}

		const std::unique_ptr<FrameBuffer> &buffer = buffers[i];
		int ret = request->addBuffer(stream, buffer.get());
		if (ret < 0)
		{
			std::cerr << "Can't set buffer for request"
				  << std::endl;
			return EXIT_FAILURE;
		}

		/*
		 * Controls can be added to a request on a per frame basis.
		 */
		ControlList &controls = request->controls();
		controls.set(controls::Brightness, 0.5);

		requests.push_back(std::move(request));
	}



    camera->requestCompleted.connect(requestComplete);




    camera->start();
	for (std::unique_ptr<Request> &request : requests)
		camera->queueRequest(request.get());




    loop.timeout(3);
	int ret = loop.exec();
	std::cout << "Capture ran for " << 3 << " seconds and "
		  << "stopped with exit status: " << ret << std::endl;


    camera->stop();
	allocator->free(stream);
	delete allocator;
	camera->release();
	camera.reset();
	cameraManager->stop();
}

void Cam::stop()
{
    camera->release();
}