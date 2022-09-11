/* SPDX-License-Identifier: LGPL-2.1-or-later */
/*
 * Copyright (C) 2020, Google Inc.
 *
 * raspberrypi_ipa_proxy.h - Image Processing Algorithm proxy for raspberrypi
 *
 * This file is auto-generated. Do not edit.
 */

#pragma once

#include <libcamera/ipa/ipa_interface.h>
#include <libcamera/ipa/raspberrypi_ipa_interface.h>

#include <libcamera/base/thread.h>

#include "libcamera/internal/control_serializer.h"
#include "libcamera/internal/ipa_proxy.h"
#include "libcamera/internal/ipc_pipe.h"
#include "libcamera/internal/ipc_pipe_unixsocket.h"
#include "libcamera/internal/ipc_unixsocket.h"

namespace libcamera {

namespace ipa {

namespace RPi {


class IPAProxyRPi : public IPAProxy, public IPARPiInterface, public Object
{
public:
	IPAProxyRPi(IPAModule *ipam, bool isolate);
	~IPAProxyRPi();


        int32_t init(
        	const IPASettings &settings,
        	IPAInitResult *result) override;

        void start(
        	const ControlList &controls,
        	StartConfig *startConfig) override;

        void stop() override;

        int32_t configure(
        	const IPACameraSensorInfo &sensorInfo,
        	const std::map<uint32_t, libcamera::IPAStream> &streamConfig,
        	const std::map<uint32_t, libcamera::ControlInfoMap> &entityControls,
        	const IPAConfig &ipaConfig,
        	ControlList *controls,
        	IPAConfigResult *result) override;

        void mapBuffers(
        	const std::vector<libcamera::IPABuffer> &buffers) override;

        void unmapBuffers(
        	const std::vector<uint32_t> &ids) override;

        void signalStatReady(
        	const uint32_t bufferId) override;

        void signalQueueRequest(
        	const ControlList &controls) override;

        void signalIspPrepare(
        	const ISPConfig &data) override;

	Signal<uint32_t, const ControlList &> statsMetadataComplete;

	Signal<uint32_t> runIsp;

	Signal<uint32_t> embeddedComplete;

	Signal<const ControlList &> setIspControls;

	Signal<const ControlList &> setDelayedControls;


private:
	void recvMessage(const IPCMessage &data);


        int32_t initThread(
        	const IPASettings &settings,
        	IPAInitResult *result);
        int32_t initIPC(
        	const IPASettings &settings,
        	IPAInitResult *result);

        void startThread(
        	const ControlList &controls,
        	StartConfig *startConfig);
        void startIPC(
        	const ControlList &controls,
        	StartConfig *startConfig);

        void stopThread();
        void stopIPC();

        int32_t configureThread(
        	const IPACameraSensorInfo &sensorInfo,
        	const std::map<uint32_t, libcamera::IPAStream> &streamConfig,
        	const std::map<uint32_t, libcamera::ControlInfoMap> &entityControls,
        	const IPAConfig &ipaConfig,
        	ControlList *controls,
        	IPAConfigResult *result);
        int32_t configureIPC(
        	const IPACameraSensorInfo &sensorInfo,
        	const std::map<uint32_t, libcamera::IPAStream> &streamConfig,
        	const std::map<uint32_t, libcamera::ControlInfoMap> &entityControls,
        	const IPAConfig &ipaConfig,
        	ControlList *controls,
        	IPAConfigResult *result);

        void mapBuffersThread(
        	const std::vector<libcamera::IPABuffer> &buffers);
        void mapBuffersIPC(
        	const std::vector<libcamera::IPABuffer> &buffers);

        void unmapBuffersThread(
        	const std::vector<uint32_t> &ids);
        void unmapBuffersIPC(
        	const std::vector<uint32_t> &ids);

        void signalStatReadyThread(
        	const uint32_t bufferId);
        void signalStatReadyIPC(
        	const uint32_t bufferId);

        void signalQueueRequestThread(
        	const ControlList &controls);
        void signalQueueRequestIPC(
        	const ControlList &controls);

        void signalIspPrepareThread(
        	const ISPConfig &data);
        void signalIspPrepareIPC(
        	const ISPConfig &data);


        void statsMetadataCompleteThread(
        	const uint32_t bufferId,
        	const ControlList &controls);
	void statsMetadataCompleteIPC(
		std::vector<uint8_t>::const_iterator data,
		size_t dataSize,
		const std::vector<SharedFD> &fds);

        void runIspThread(
        	const uint32_t bufferId);
	void runIspIPC(
		std::vector<uint8_t>::const_iterator data,
		size_t dataSize,
		const std::vector<SharedFD> &fds);

        void embeddedCompleteThread(
        	const uint32_t bufferId);
	void embeddedCompleteIPC(
		std::vector<uint8_t>::const_iterator data,
		size_t dataSize,
		const std::vector<SharedFD> &fds);

        void setIspControlsThread(
        	const ControlList &controls);
	void setIspControlsIPC(
		std::vector<uint8_t>::const_iterator data,
		size_t dataSize,
		const std::vector<SharedFD> &fds);

        void setDelayedControlsThread(
        	const ControlList &controls);
	void setDelayedControlsIPC(
		std::vector<uint8_t>::const_iterator data,
		size_t dataSize,
		const std::vector<SharedFD> &fds);


	/* Helper class to invoke async functions in another thread. */
	class ThreadProxy : public Object
	{
	public:
		ThreadProxy()
			: ipa_(nullptr)
		{
		}

		void setIPA(IPARPiInterface *ipa)
		{
			ipa_ = ipa;
		}

		void stop()
		{
			ipa_->stop();
		}

		void start(
                	const ControlList &controls,
                	StartConfig *startConfig)
		{
			ipa_->start(controls, startConfig);
		}
		void signalStatReady(
                	const uint32_t bufferId)
		{
			ipa_->signalStatReady(bufferId);
		}
		void signalQueueRequest(
                	const ControlList &controls)
		{
			ipa_->signalQueueRequest(controls);
		}
		void signalIspPrepare(
                	const ISPConfig &data)
		{
			ipa_->signalIspPrepare(data);
		}

	private:
		IPARPiInterface *ipa_;
	};

	Thread thread_;
	ThreadProxy proxy_;
	std::unique_ptr<IPARPiInterface> ipa_;

	const bool isolate_;

	std::unique_ptr<IPCPipeUnixSocket> ipc_;

	ControlSerializer controlSerializer_;


	uint32_t seq_;
};

} /* namespace RPi */

} /* namespace ipa */

} /* namespace libcamera */