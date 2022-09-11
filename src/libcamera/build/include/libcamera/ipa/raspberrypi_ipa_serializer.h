/* SPDX-License-Identifier: LGPL-2.1-or-later */
/*
 * Copyright (C) 2020, Google Inc.
 *
 * raspberrypi_ipa_serializer.h - Image Processing Algorithm data serializer for raspberrypi
 *
 * This file is auto-generated. Do not edit.
 */

#pragma once

#include <tuple>
#include <vector>

#include <libcamera/ipa/raspberrypi_ipa_interface.h>
#include <libcamera/ipa/core_ipa_serializer.h>

#include "libcamera/internal/control_serializer.h"
#include "libcamera/internal/ipa_data_serializer.h"

namespace libcamera {

LOG_DECLARE_CATEGORY(IPADataSerializer)

template<>
class IPADataSerializer<ipa::RPi::SensorConfig>
{
public:
	static std::tuple<std::vector<uint8_t>, std::vector<SharedFD>>
	serialize(const ipa::RPi::SensorConfig &data,
		  [[maybe_unused]] ControlSerializer *cs = nullptr)
	{
		std::vector<uint8_t> retData;

		std::vector<uint8_t> gainDelay;
		std::tie(gainDelay, std::ignore) =
			IPADataSerializer<uint32_t>::serialize(data.gainDelay);
		retData.insert(retData.end(), gainDelay.begin(), gainDelay.end());

		std::vector<uint8_t> exposureDelay;
		std::tie(exposureDelay, std::ignore) =
			IPADataSerializer<uint32_t>::serialize(data.exposureDelay);
		retData.insert(retData.end(), exposureDelay.begin(), exposureDelay.end());

		std::vector<uint8_t> vblankDelay;
		std::tie(vblankDelay, std::ignore) =
			IPADataSerializer<uint32_t>::serialize(data.vblankDelay);
		retData.insert(retData.end(), vblankDelay.begin(), vblankDelay.end());

		std::vector<uint8_t> sensorMetadata;
		std::tie(sensorMetadata, std::ignore) =
			IPADataSerializer<uint32_t>::serialize(data.sensorMetadata);
		retData.insert(retData.end(), sensorMetadata.begin(), sensorMetadata.end());

		return {retData, {}};
	}

	static ipa::RPi::SensorConfig
	deserialize(std::vector<uint8_t> &data,
		    ControlSerializer *cs = nullptr)
	{
		return IPADataSerializer<ipa::RPi::SensorConfig>::deserialize(data.cbegin(), data.cend(), cs);
	}


	static ipa::RPi::SensorConfig
	deserialize(std::vector<uint8_t>::const_iterator dataBegin,
		    std::vector<uint8_t>::const_iterator dataEnd,
		    [[maybe_unused]] ControlSerializer *cs = nullptr)
	{
		ipa::RPi::SensorConfig ret;
		std::vector<uint8_t>::const_iterator m = dataBegin;

		size_t dataSize = std::distance(dataBegin, dataEnd);

		if (dataSize < 4) {
			LOG(IPADataSerializer, Error)
				<< "Failed to deserialize " << "gainDelay"
				<< ": not enough data, expected "
				<< (4) << ", got " << (dataSize);
			return ret;
		}
		ret.gainDelay = IPADataSerializer<uint32_t>::deserialize(m, m + 4);
		m += 4;
		dataSize -= 4;


		if (dataSize < 4) {
			LOG(IPADataSerializer, Error)
				<< "Failed to deserialize " << "exposureDelay"
				<< ": not enough data, expected "
				<< (4) << ", got " << (dataSize);
			return ret;
		}
		ret.exposureDelay = IPADataSerializer<uint32_t>::deserialize(m, m + 4);
		m += 4;
		dataSize -= 4;


		if (dataSize < 4) {
			LOG(IPADataSerializer, Error)
				<< "Failed to deserialize " << "vblankDelay"
				<< ": not enough data, expected "
				<< (4) << ", got " << (dataSize);
			return ret;
		}
		ret.vblankDelay = IPADataSerializer<uint32_t>::deserialize(m, m + 4);
		m += 4;
		dataSize -= 4;


		if (dataSize < 4) {
			LOG(IPADataSerializer, Error)
				<< "Failed to deserialize " << "sensorMetadata"
				<< ": not enough data, expected "
				<< (4) << ", got " << (dataSize);
			return ret;
		}
		ret.sensorMetadata = IPADataSerializer<uint32_t>::deserialize(m, m + 4);

		return ret;
	}

	static ipa::RPi::SensorConfig
	deserialize(std::vector<uint8_t> &data,
		    [[maybe_unused]] std::vector<SharedFD> &fds,
		    ControlSerializer *cs = nullptr)
	{
		return IPADataSerializer<ipa::RPi::SensorConfig>::deserialize(data.cbegin(), data.cend(), cs);
	}

	static ipa::RPi::SensorConfig
	deserialize(std::vector<uint8_t>::const_iterator dataBegin,
		    std::vector<uint8_t>::const_iterator dataEnd,
		    [[maybe_unused]] std::vector<SharedFD>::const_iterator fdsBegin,
		    [[maybe_unused]] std::vector<SharedFD>::const_iterator fdsEnd,
		    ControlSerializer *cs = nullptr)
	{
		return IPADataSerializer<ipa::RPi::SensorConfig>::deserialize(dataBegin, dataEnd, cs);
	}
};

template<>
class IPADataSerializer<ipa::RPi::IPAInitResult>
{
public:
	static std::tuple<std::vector<uint8_t>, std::vector<SharedFD>>
	serialize(const ipa::RPi::IPAInitResult &data,
		  ControlSerializer *cs)
	{
		std::vector<uint8_t> retData;

		std::vector<uint8_t> sensorConfig;
		std::tie(sensorConfig, std::ignore) =
			IPADataSerializer<ipa::RPi::SensorConfig>::serialize(data.sensorConfig, cs);
		appendPOD<uint32_t>(retData, sensorConfig.size());
		retData.insert(retData.end(), sensorConfig.begin(), sensorConfig.end());

		if (data.controlInfo.size() > 0) {
			std::vector<uint8_t> controlInfo;
			std::tie(controlInfo, std::ignore) =
				IPADataSerializer<ControlInfoMap>::serialize(data.controlInfo, cs);
			appendPOD<uint32_t>(retData, controlInfo.size());
			retData.insert(retData.end(), controlInfo.begin(), controlInfo.end());
		} else {
			appendPOD<uint32_t>(retData, 0);
		}

		return {retData, {}};
	}

	static ipa::RPi::IPAInitResult
	deserialize(std::vector<uint8_t> &data,
		    ControlSerializer *cs)
	{
		return IPADataSerializer<ipa::RPi::IPAInitResult>::deserialize(data.cbegin(), data.cend(), cs);
	}


	static ipa::RPi::IPAInitResult
	deserialize(std::vector<uint8_t>::const_iterator dataBegin,
		    std::vector<uint8_t>::const_iterator dataEnd,
		    ControlSerializer *cs)
	{
		ipa::RPi::IPAInitResult ret;
		std::vector<uint8_t>::const_iterator m = dataBegin;

		size_t dataSize = std::distance(dataBegin, dataEnd);

		if (dataSize < 4) {
			LOG(IPADataSerializer, Error)
				<< "Failed to deserialize " << "sensorConfigSize"
				<< ": not enough data, expected "
				<< (4) << ", got " << (dataSize);
			return ret;
		}
		const size_t sensorConfigSize = readPOD<uint32_t>(m, 0, dataEnd);
		m += 4;
		dataSize -= 4;
		if (dataSize < sensorConfigSize) {
			LOG(IPADataSerializer, Error)
				<< "Failed to deserialize " << "sensorConfig"
				<< ": not enough data, expected "
				<< (sensorConfigSize) << ", got " << (dataSize);
			return ret;
		}
		ret.sensorConfig =
			IPADataSerializer<ipa::RPi::SensorConfig>::deserialize(m, m + sensorConfigSize, cs);
		m += sensorConfigSize;
		dataSize -= sensorConfigSize;


		if (dataSize < 4) {
			LOG(IPADataSerializer, Error)
				<< "Failed to deserialize " << "controlInfoSize"
				<< ": not enough data, expected "
				<< (4) << ", got " << (dataSize);
			return ret;
		}
		const size_t controlInfoSize = readPOD<uint32_t>(m, 0, dataEnd);
		m += 4;
		dataSize -= 4;
		if (dataSize < controlInfoSize) {
			LOG(IPADataSerializer, Error)
				<< "Failed to deserialize " << "controlInfo"
				<< ": not enough data, expected "
				<< (controlInfoSize) << ", got " << (dataSize);
			return ret;
		}
		if (controlInfoSize > 0)
			ret.controlInfo =
				IPADataSerializer<ControlInfoMap>::deserialize(m, m + controlInfoSize, cs);

		return ret;
	}

	static ipa::RPi::IPAInitResult
	deserialize(std::vector<uint8_t> &data,
		    [[maybe_unused]] std::vector<SharedFD> &fds,
		    ControlSerializer *cs = nullptr)
	{
		return IPADataSerializer<ipa::RPi::IPAInitResult>::deserialize(data.cbegin(), data.cend(), cs);
	}

	static ipa::RPi::IPAInitResult
	deserialize(std::vector<uint8_t>::const_iterator dataBegin,
		    std::vector<uint8_t>::const_iterator dataEnd,
		    [[maybe_unused]] std::vector<SharedFD>::const_iterator fdsBegin,
		    [[maybe_unused]] std::vector<SharedFD>::const_iterator fdsEnd,
		    ControlSerializer *cs = nullptr)
	{
		return IPADataSerializer<ipa::RPi::IPAInitResult>::deserialize(dataBegin, dataEnd, cs);
	}
};

template<>
class IPADataSerializer<ipa::RPi::ISPConfig>
{
public:
	static std::tuple<std::vector<uint8_t>, std::vector<SharedFD>>
	serialize(const ipa::RPi::ISPConfig &data,
		  ControlSerializer *cs)
	{
		std::vector<uint8_t> retData;

		std::vector<uint8_t> embeddedBufferId;
		std::tie(embeddedBufferId, std::ignore) =
			IPADataSerializer<uint32_t>::serialize(data.embeddedBufferId);
		retData.insert(retData.end(), embeddedBufferId.begin(), embeddedBufferId.end());

		std::vector<uint8_t> bayerBufferId;
		std::tie(bayerBufferId, std::ignore) =
			IPADataSerializer<uint32_t>::serialize(data.bayerBufferId);
		retData.insert(retData.end(), bayerBufferId.begin(), bayerBufferId.end());

		std::vector<uint8_t> embeddedBufferPresent;
		std::tie(embeddedBufferPresent, std::ignore) =
			IPADataSerializer<bool>::serialize(data.embeddedBufferPresent);
		retData.insert(retData.end(), embeddedBufferPresent.begin(), embeddedBufferPresent.end());

		if (data.controls.size() > 0) {
			std::vector<uint8_t> controls;
			std::tie(controls, std::ignore) =
				IPADataSerializer<ControlList>::serialize(data.controls, cs);
			appendPOD<uint32_t>(retData, controls.size());
			retData.insert(retData.end(), controls.begin(), controls.end());
		} else {
			appendPOD<uint32_t>(retData, 0);
		}

		return {retData, {}};
	}

	static ipa::RPi::ISPConfig
	deserialize(std::vector<uint8_t> &data,
		    ControlSerializer *cs)
	{
		return IPADataSerializer<ipa::RPi::ISPConfig>::deserialize(data.cbegin(), data.cend(), cs);
	}


	static ipa::RPi::ISPConfig
	deserialize(std::vector<uint8_t>::const_iterator dataBegin,
		    std::vector<uint8_t>::const_iterator dataEnd,
		    ControlSerializer *cs)
	{
		ipa::RPi::ISPConfig ret;
		std::vector<uint8_t>::const_iterator m = dataBegin;

		size_t dataSize = std::distance(dataBegin, dataEnd);

		if (dataSize < 4) {
			LOG(IPADataSerializer, Error)
				<< "Failed to deserialize " << "embeddedBufferId"
				<< ": not enough data, expected "
				<< (4) << ", got " << (dataSize);
			return ret;
		}
		ret.embeddedBufferId = IPADataSerializer<uint32_t>::deserialize(m, m + 4);
		m += 4;
		dataSize -= 4;


		if (dataSize < 4) {
			LOG(IPADataSerializer, Error)
				<< "Failed to deserialize " << "bayerBufferId"
				<< ": not enough data, expected "
				<< (4) << ", got " << (dataSize);
			return ret;
		}
		ret.bayerBufferId = IPADataSerializer<uint32_t>::deserialize(m, m + 4);
		m += 4;
		dataSize -= 4;


		if (dataSize < 1) {
			LOG(IPADataSerializer, Error)
				<< "Failed to deserialize " << "embeddedBufferPresent"
				<< ": not enough data, expected "
				<< (1) << ", got " << (dataSize);
			return ret;
		}
		ret.embeddedBufferPresent = IPADataSerializer<bool>::deserialize(m, m + 1);
		m += 1;
		dataSize -= 1;


		if (dataSize < 4) {
			LOG(IPADataSerializer, Error)
				<< "Failed to deserialize " << "controlsSize"
				<< ": not enough data, expected "
				<< (4) << ", got " << (dataSize);
			return ret;
		}
		const size_t controlsSize = readPOD<uint32_t>(m, 0, dataEnd);
		m += 4;
		dataSize -= 4;
		if (dataSize < controlsSize) {
			LOG(IPADataSerializer, Error)
				<< "Failed to deserialize " << "controls"
				<< ": not enough data, expected "
				<< (controlsSize) << ", got " << (dataSize);
			return ret;
		}
		if (controlsSize > 0)
			ret.controls =
				IPADataSerializer<ControlList>::deserialize(m, m + controlsSize, cs);

		return ret;
	}

	static ipa::RPi::ISPConfig
	deserialize(std::vector<uint8_t> &data,
		    [[maybe_unused]] std::vector<SharedFD> &fds,
		    ControlSerializer *cs = nullptr)
	{
		return IPADataSerializer<ipa::RPi::ISPConfig>::deserialize(data.cbegin(), data.cend(), cs);
	}

	static ipa::RPi::ISPConfig
	deserialize(std::vector<uint8_t>::const_iterator dataBegin,
		    std::vector<uint8_t>::const_iterator dataEnd,
		    [[maybe_unused]] std::vector<SharedFD>::const_iterator fdsBegin,
		    [[maybe_unused]] std::vector<SharedFD>::const_iterator fdsEnd,
		    ControlSerializer *cs = nullptr)
	{
		return IPADataSerializer<ipa::RPi::ISPConfig>::deserialize(dataBegin, dataEnd, cs);
	}
};

template<>
class IPADataSerializer<ipa::RPi::IPAConfig>
{
public:
	static std::tuple<std::vector<uint8_t>, std::vector<SharedFD>>
	serialize(const ipa::RPi::IPAConfig &data,
		  [[maybe_unused]] ControlSerializer *cs = nullptr)
	{
		std::vector<uint8_t> retData;
		std::vector<SharedFD> retFds;

		std::vector<uint8_t> transform;
		std::tie(transform, std::ignore) =
			IPADataSerializer<uint32_t>::serialize(data.transform);
		retData.insert(retData.end(), transform.begin(), transform.end());

		std::vector<uint8_t> lsTableHandle;
		std::vector<SharedFD> lsTableHandleFds;
		std::tie(lsTableHandle, lsTableHandleFds) =
			IPADataSerializer<SharedFD>::serialize(data.lsTableHandle);
		retData.insert(retData.end(), lsTableHandle.begin(), lsTableHandle.end());
		retFds.insert(retFds.end(), lsTableHandleFds.begin(), lsTableHandleFds.end());

		return {retData, retFds};
	}

	static ipa::RPi::IPAConfig
	deserialize(std::vector<uint8_t> &data,
		    std::vector<SharedFD> &fds,
		    ControlSerializer *cs = nullptr)
	{
		return IPADataSerializer<ipa::RPi::IPAConfig>::deserialize(data.cbegin(), data.cend(), fds.cbegin(), fds.cend(), cs);
	}


	static ipa::RPi::IPAConfig
	deserialize(std::vector<uint8_t>::const_iterator dataBegin,
		    std::vector<uint8_t>::const_iterator dataEnd,
		    std::vector<SharedFD>::const_iterator fdsBegin,
		    std::vector<SharedFD>::const_iterator fdsEnd,
		    [[maybe_unused]] ControlSerializer *cs = nullptr)
	{
		ipa::RPi::IPAConfig ret;
		std::vector<uint8_t>::const_iterator m = dataBegin;
		std::vector<SharedFD>::const_iterator n = fdsBegin;

		size_t dataSize = std::distance(dataBegin, dataEnd);
		[[maybe_unused]] size_t fdsSize = std::distance(fdsBegin, fdsEnd);

		if (dataSize < 4) {
			LOG(IPADataSerializer, Error)
				<< "Failed to deserialize " << "transform"
				<< ": not enough data, expected "
				<< (4) << ", got " << (dataSize);
			return ret;
		}
		ret.transform = IPADataSerializer<uint32_t>::deserialize(m, m + 4);
		m += 4;
		dataSize -= 4;


		if (dataSize < 4) {
			LOG(IPADataSerializer, Error)
				<< "Failed to deserialize " << "lsTableHandle"
				<< ": not enough data, expected "
				<< (4) << ", got " << (dataSize);
			return ret;
		}
		ret.lsTableHandle = IPADataSerializer<SharedFD>::deserialize(m, m + 4, n, n + 1, cs);

		return ret;
	}
};

template<>
class IPADataSerializer<ipa::RPi::IPAConfigResult>
{
public:
	static std::tuple<std::vector<uint8_t>, std::vector<SharedFD>>
	serialize(const ipa::RPi::IPAConfigResult &data,
		  ControlSerializer *cs)
	{
		std::vector<uint8_t> retData;

		std::vector<uint8_t> modeSensitivity;
		std::tie(modeSensitivity, std::ignore) =
			IPADataSerializer<float>::serialize(data.modeSensitivity);
		retData.insert(retData.end(), modeSensitivity.begin(), modeSensitivity.end());

		if (data.controlInfo.size() > 0) {
			std::vector<uint8_t> controlInfo;
			std::tie(controlInfo, std::ignore) =
				IPADataSerializer<ControlInfoMap>::serialize(data.controlInfo, cs);
			appendPOD<uint32_t>(retData, controlInfo.size());
			retData.insert(retData.end(), controlInfo.begin(), controlInfo.end());
		} else {
			appendPOD<uint32_t>(retData, 0);
		}

		return {retData, {}};
	}

	static ipa::RPi::IPAConfigResult
	deserialize(std::vector<uint8_t> &data,
		    ControlSerializer *cs)
	{
		return IPADataSerializer<ipa::RPi::IPAConfigResult>::deserialize(data.cbegin(), data.cend(), cs);
	}


	static ipa::RPi::IPAConfigResult
	deserialize(std::vector<uint8_t>::const_iterator dataBegin,
		    std::vector<uint8_t>::const_iterator dataEnd,
		    ControlSerializer *cs)
	{
		ipa::RPi::IPAConfigResult ret;
		std::vector<uint8_t>::const_iterator m = dataBegin;

		size_t dataSize = std::distance(dataBegin, dataEnd);

		if (dataSize < 4) {
			LOG(IPADataSerializer, Error)
				<< "Failed to deserialize " << "modeSensitivity"
				<< ": not enough data, expected "
				<< (4) << ", got " << (dataSize);
			return ret;
		}
		ret.modeSensitivity = IPADataSerializer<float>::deserialize(m, m + 4);
		m += 4;
		dataSize -= 4;


		if (dataSize < 4) {
			LOG(IPADataSerializer, Error)
				<< "Failed to deserialize " << "controlInfoSize"
				<< ": not enough data, expected "
				<< (4) << ", got " << (dataSize);
			return ret;
		}
		const size_t controlInfoSize = readPOD<uint32_t>(m, 0, dataEnd);
		m += 4;
		dataSize -= 4;
		if (dataSize < controlInfoSize) {
			LOG(IPADataSerializer, Error)
				<< "Failed to deserialize " << "controlInfo"
				<< ": not enough data, expected "
				<< (controlInfoSize) << ", got " << (dataSize);
			return ret;
		}
		if (controlInfoSize > 0)
			ret.controlInfo =
				IPADataSerializer<ControlInfoMap>::deserialize(m, m + controlInfoSize, cs);

		return ret;
	}

	static ipa::RPi::IPAConfigResult
	deserialize(std::vector<uint8_t> &data,
		    [[maybe_unused]] std::vector<SharedFD> &fds,
		    ControlSerializer *cs = nullptr)
	{
		return IPADataSerializer<ipa::RPi::IPAConfigResult>::deserialize(data.cbegin(), data.cend(), cs);
	}

	static ipa::RPi::IPAConfigResult
	deserialize(std::vector<uint8_t>::const_iterator dataBegin,
		    std::vector<uint8_t>::const_iterator dataEnd,
		    [[maybe_unused]] std::vector<SharedFD>::const_iterator fdsBegin,
		    [[maybe_unused]] std::vector<SharedFD>::const_iterator fdsEnd,
		    ControlSerializer *cs = nullptr)
	{
		return IPADataSerializer<ipa::RPi::IPAConfigResult>::deserialize(dataBegin, dataEnd, cs);
	}
};

template<>
class IPADataSerializer<ipa::RPi::StartConfig>
{
public:
	static std::tuple<std::vector<uint8_t>, std::vector<SharedFD>>
	serialize(const ipa::RPi::StartConfig &data,
		  ControlSerializer *cs)
	{
		std::vector<uint8_t> retData;

		if (data.controls.size() > 0) {
			std::vector<uint8_t> controls;
			std::tie(controls, std::ignore) =
				IPADataSerializer<ControlList>::serialize(data.controls, cs);
			appendPOD<uint32_t>(retData, controls.size());
			retData.insert(retData.end(), controls.begin(), controls.end());
		} else {
			appendPOD<uint32_t>(retData, 0);
		}

		std::vector<uint8_t> dropFrameCount;
		std::tie(dropFrameCount, std::ignore) =
			IPADataSerializer<int32_t>::serialize(data.dropFrameCount);
		retData.insert(retData.end(), dropFrameCount.begin(), dropFrameCount.end());

		std::vector<uint8_t> maxSensorFrameLengthMs;
		std::tie(maxSensorFrameLengthMs, std::ignore) =
			IPADataSerializer<uint32_t>::serialize(data.maxSensorFrameLengthMs);
		retData.insert(retData.end(), maxSensorFrameLengthMs.begin(), maxSensorFrameLengthMs.end());

		return {retData, {}};
	}

	static ipa::RPi::StartConfig
	deserialize(std::vector<uint8_t> &data,
		    ControlSerializer *cs)
	{
		return IPADataSerializer<ipa::RPi::StartConfig>::deserialize(data.cbegin(), data.cend(), cs);
	}


	static ipa::RPi::StartConfig
	deserialize(std::vector<uint8_t>::const_iterator dataBegin,
		    std::vector<uint8_t>::const_iterator dataEnd,
		    ControlSerializer *cs)
	{
		ipa::RPi::StartConfig ret;
		std::vector<uint8_t>::const_iterator m = dataBegin;

		size_t dataSize = std::distance(dataBegin, dataEnd);

		if (dataSize < 4) {
			LOG(IPADataSerializer, Error)
				<< "Failed to deserialize " << "controlsSize"
				<< ": not enough data, expected "
				<< (4) << ", got " << (dataSize);
			return ret;
		}
		const size_t controlsSize = readPOD<uint32_t>(m, 0, dataEnd);
		m += 4;
		dataSize -= 4;
		if (dataSize < controlsSize) {
			LOG(IPADataSerializer, Error)
				<< "Failed to deserialize " << "controls"
				<< ": not enough data, expected "
				<< (controlsSize) << ", got " << (dataSize);
			return ret;
		}
		if (controlsSize > 0)
			ret.controls =
				IPADataSerializer<ControlList>::deserialize(m, m + controlsSize, cs);
		m += controlsSize;
		dataSize -= controlsSize;


		if (dataSize < 4) {
			LOG(IPADataSerializer, Error)
				<< "Failed to deserialize " << "dropFrameCount"
				<< ": not enough data, expected "
				<< (4) << ", got " << (dataSize);
			return ret;
		}
		ret.dropFrameCount = IPADataSerializer<int32_t>::deserialize(m, m + 4);
		m += 4;
		dataSize -= 4;


		if (dataSize < 4) {
			LOG(IPADataSerializer, Error)
				<< "Failed to deserialize " << "maxSensorFrameLengthMs"
				<< ": not enough data, expected "
				<< (4) << ", got " << (dataSize);
			return ret;
		}
		ret.maxSensorFrameLengthMs = IPADataSerializer<uint32_t>::deserialize(m, m + 4);

		return ret;
	}

	static ipa::RPi::StartConfig
	deserialize(std::vector<uint8_t> &data,
		    [[maybe_unused]] std::vector<SharedFD> &fds,
		    ControlSerializer *cs = nullptr)
	{
		return IPADataSerializer<ipa::RPi::StartConfig>::deserialize(data.cbegin(), data.cend(), cs);
	}

	static ipa::RPi::StartConfig
	deserialize(std::vector<uint8_t>::const_iterator dataBegin,
		    std::vector<uint8_t>::const_iterator dataEnd,
		    [[maybe_unused]] std::vector<SharedFD>::const_iterator fdsBegin,
		    [[maybe_unused]] std::vector<SharedFD>::const_iterator fdsEnd,
		    ControlSerializer *cs = nullptr)
	{
		return IPADataSerializer<ipa::RPi::StartConfig>::deserialize(dataBegin, dataEnd, cs);
	}
};


} /* namespace libcamera */