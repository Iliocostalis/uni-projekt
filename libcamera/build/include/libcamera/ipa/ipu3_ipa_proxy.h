/* SPDX-License-Identifier: LGPL-2.1-or-later */
/*
 * Copyright (C) 2020, Google Inc.
 *
 * ipu3_ipa_proxy.h - Image Processing Algorithm proxy for ipu3
 *
 * This file is auto-generated. Do not edit.
 */

#pragma once

#include <libcamera/ipa/ipa_interface.h>
#include <libcamera/ipa/ipu3_ipa_interface.h>

#include <libcamera/base/thread.h>

#include "libcamera/internal/control_serializer.h"
#include "libcamera/internal/ipa_proxy.h"
#include "libcamera/internal/ipc_pipe.h"
#include "libcamera/internal/ipc_pipe_unixsocket.h"
#include "libcamera/internal/ipc_unixsocket.h"

namespace libcamera {

namespace ipa {

namespace ipu3 {


class IPAProxyIPU3 : public IPAProxy, public IPAIPU3Interface, public Object
{
public:
	IPAProxyIPU3(IPAModule *ipam, bool isolate);
	~IPAProxyIPU3();


        int32_t init(
        	const IPASettings &settings,
        	const IPACameraSensorInfo &sensorInfo,
        	const ControlInfoMap &sensorControls,
        	ControlInfoMap *ipaControls) override;

        int32_t start() override;

        void stop() override;

        int32_t configure(
        	const IPAConfigInfo &configInfo,
        	ControlInfoMap *ipaControls) override;

        void mapBuffers(
        	const std::vector<libcamera::IPABuffer> &buffers) override;

        void unmapBuffers(
        	const std::vector<uint32_t> &ids) override;

        void queueRequest(
        	const uint32_t frame,
        	const ControlList &controls) override;

        void fillParamsBuffer(
        	const uint32_t frame,
        	const uint32_t bufferId) override;

        void processStatsBuffer(
        	const uint32_t frame,
        	const int64_t frameTimestamp,
        	const uint32_t bufferId,
        	const ControlList &sensorControls) override;

	Signal<uint32_t, const ControlList &, const ControlList &> setSensorControls;

	Signal<uint32_t> paramsBufferReady;

	Signal<uint32_t, const ControlList &> metadataReady;


private:
	void recvMessage(const IPCMessage &data);


        int32_t initThread(
        	const IPASettings &settings,
        	const IPACameraSensorInfo &sensorInfo,
        	const ControlInfoMap &sensorControls,
        	ControlInfoMap *ipaControls);
        int32_t initIPC(
        	const IPASettings &settings,
        	const IPACameraSensorInfo &sensorInfo,
        	const ControlInfoMap &sensorControls,
        	ControlInfoMap *ipaControls);

        int32_t startThread();
        int32_t startIPC();

        void stopThread();
        void stopIPC();

        int32_t configureThread(
        	const IPAConfigInfo &configInfo,
        	ControlInfoMap *ipaControls);
        int32_t configureIPC(
        	const IPAConfigInfo &configInfo,
        	ControlInfoMap *ipaControls);

        void mapBuffersThread(
        	const std::vector<libcamera::IPABuffer> &buffers);
        void mapBuffersIPC(
        	const std::vector<libcamera::IPABuffer> &buffers);

        void unmapBuffersThread(
        	const std::vector<uint32_t> &ids);
        void unmapBuffersIPC(
        	const std::vector<uint32_t> &ids);

        void queueRequestThread(
        	const uint32_t frame,
        	const ControlList &controls);
        void queueRequestIPC(
        	const uint32_t frame,
        	const ControlList &controls);

        void fillParamsBufferThread(
        	const uint32_t frame,
        	const uint32_t bufferId);
        void fillParamsBufferIPC(
        	const uint32_t frame,
        	const uint32_t bufferId);

        void processStatsBufferThread(
        	const uint32_t frame,
        	const int64_t frameTimestamp,
        	const uint32_t bufferId,
        	const ControlList &sensorControls);
        void processStatsBufferIPC(
        	const uint32_t frame,
        	const int64_t frameTimestamp,
        	const uint32_t bufferId,
        	const ControlList &sensorControls);


        void setSensorControlsThread(
        	const uint32_t frame,
        	const ControlList &sensorControls,
        	const ControlList &lensControls);
	void setSensorControlsIPC(
		std::vector<uint8_t>::const_iterator data,
		size_t dataSize,
		const std::vector<SharedFD> &fds);

        void paramsBufferReadyThread(
        	const uint32_t frame);
	void paramsBufferReadyIPC(
		std::vector<uint8_t>::const_iterator data,
		size_t dataSize,
		const std::vector<SharedFD> &fds);

        void metadataReadyThread(
        	const uint32_t frame,
        	const ControlList &metadata);
	void metadataReadyIPC(
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

		void setIPA(IPAIPU3Interface *ipa)
		{
			ipa_ = ipa;
		}

		void stop()
		{
			ipa_->stop();
		}

		int32_t start()
		{
			return ipa_->start();
		}
		void queueRequest(
                	const uint32_t frame,
                	const ControlList &controls)
		{
			ipa_->queueRequest(frame, controls);
		}
		void fillParamsBuffer(
                	const uint32_t frame,
                	const uint32_t bufferId)
		{
			ipa_->fillParamsBuffer(frame, bufferId);
		}
		void processStatsBuffer(
                	const uint32_t frame,
                	const int64_t frameTimestamp,
                	const uint32_t bufferId,
                	const ControlList &sensorControls)
		{
			ipa_->processStatsBuffer(frame, frameTimestamp, bufferId, sensorControls);
		}

	private:
		IPAIPU3Interface *ipa_;
	};

	Thread thread_;
	ThreadProxy proxy_;
	std::unique_ptr<IPAIPU3Interface> ipa_;

	const bool isolate_;

	std::unique_ptr<IPCPipeUnixSocket> ipc_;

	ControlSerializer controlSerializer_;


	uint32_t seq_;
};

} /* namespace ipu3 */

} /* namespace ipa */

} /* namespace libcamera */