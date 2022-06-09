/* SPDX-License-Identifier: LGPL-2.1-or-later */
/*
 * Copyright (C) 2020, Google Inc.
 *
 * ipu3_ipa_serializer.h - Image Processing Algorithm data serializer for ipu3
 *
 * This file is auto-generated. Do not edit.
 */

#pragma once

#include <tuple>
#include <vector>

#include <libcamera/ipa/ipu3_ipa_interface.h>
#include <libcamera/ipa/core_ipa_serializer.h>

#include "libcamera/internal/control_serializer.h"
#include "libcamera/internal/ipa_data_serializer.h"

namespace libcamera {

LOG_DECLARE_CATEGORY(IPADataSerializer)

template<>
class IPADataSerializer<ipa::ipu3::IPAConfigInfo>
{
public:
	static std::tuple<std::vector<uint8_t>, std::vector<SharedFD>>
	serialize(const ipa::ipu3::IPAConfigInfo &data,
		  ControlSerializer *cs)
	{
		std::vector<uint8_t> retData;

		std::vector<uint8_t> sensorInfo;
		std::tie(sensorInfo, std::ignore) =
			IPADataSerializer<libcamera::IPACameraSensorInfo>::serialize(data.sensorInfo, cs);
		appendPOD<uint32_t>(retData, sensorInfo.size());
		retData.insert(retData.end(), sensorInfo.begin(), sensorInfo.end());

		if (data.sensorControls.size() > 0) {
			std::vector<uint8_t> sensorControls;
			std::tie(sensorControls, std::ignore) =
				IPADataSerializer<ControlInfoMap>::serialize(data.sensorControls, cs);
			appendPOD<uint32_t>(retData, sensorControls.size());
			retData.insert(retData.end(), sensorControls.begin(), sensorControls.end());
		} else {
			appendPOD<uint32_t>(retData, 0);
		}

		if (data.lensControls.size() > 0) {
			std::vector<uint8_t> lensControls;
			std::tie(lensControls, std::ignore) =
				IPADataSerializer<ControlInfoMap>::serialize(data.lensControls, cs);
			appendPOD<uint32_t>(retData, lensControls.size());
			retData.insert(retData.end(), lensControls.begin(), lensControls.end());
		} else {
			appendPOD<uint32_t>(retData, 0);
		}

		std::vector<uint8_t> bdsOutputSize;
		std::tie(bdsOutputSize, std::ignore) =
			IPADataSerializer<libcamera::Size>::serialize(data.bdsOutputSize, cs);
		appendPOD<uint32_t>(retData, bdsOutputSize.size());
		retData.insert(retData.end(), bdsOutputSize.begin(), bdsOutputSize.end());

		std::vector<uint8_t> iif;
		std::tie(iif, std::ignore) =
			IPADataSerializer<libcamera::Size>::serialize(data.iif, cs);
		appendPOD<uint32_t>(retData, iif.size());
		retData.insert(retData.end(), iif.begin(), iif.end());

		return {retData, {}};
	}

	static ipa::ipu3::IPAConfigInfo
	deserialize(std::vector<uint8_t> &data,
		    ControlSerializer *cs)
	{
		return IPADataSerializer<ipa::ipu3::IPAConfigInfo>::deserialize(data.cbegin(), data.cend(), cs);
	}


	static ipa::ipu3::IPAConfigInfo
	deserialize(std::vector<uint8_t>::const_iterator dataBegin,
		    std::vector<uint8_t>::const_iterator dataEnd,
		    ControlSerializer *cs)
	{
		ipa::ipu3::IPAConfigInfo ret;
		std::vector<uint8_t>::const_iterator m = dataBegin;

		size_t dataSize = std::distance(dataBegin, dataEnd);

		if (dataSize < 4) {
			LOG(IPADataSerializer, Error)
				<< "Failed to deserialize " << "sensorInfoSize"
				<< ": not enough data, expected "
				<< (4) << ", got " << (dataSize);
			return ret;
		}
		const size_t sensorInfoSize = readPOD<uint32_t>(m, 0, dataEnd);
		m += 4;
		dataSize -= 4;
		if (dataSize < sensorInfoSize) {
			LOG(IPADataSerializer, Error)
				<< "Failed to deserialize " << "sensorInfo"
				<< ": not enough data, expected "
				<< (sensorInfoSize) << ", got " << (dataSize);
			return ret;
		}
		ret.sensorInfo =
			IPADataSerializer<libcamera::IPACameraSensorInfo>::deserialize(m, m + sensorInfoSize, cs);
		m += sensorInfoSize;
		dataSize -= sensorInfoSize;


		if (dataSize < 4) {
			LOG(IPADataSerializer, Error)
				<< "Failed to deserialize " << "sensorControlsSize"
				<< ": not enough data, expected "
				<< (4) << ", got " << (dataSize);
			return ret;
		}
		const size_t sensorControlsSize = readPOD<uint32_t>(m, 0, dataEnd);
		m += 4;
		dataSize -= 4;
		if (dataSize < sensorControlsSize) {
			LOG(IPADataSerializer, Error)
				<< "Failed to deserialize " << "sensorControls"
				<< ": not enough data, expected "
				<< (sensorControlsSize) << ", got " << (dataSize);
			return ret;
		}
		if (sensorControlsSize > 0)
			ret.sensorControls =
				IPADataSerializer<ControlInfoMap>::deserialize(m, m + sensorControlsSize, cs);
		m += sensorControlsSize;
		dataSize -= sensorControlsSize;


		if (dataSize < 4) {
			LOG(IPADataSerializer, Error)
				<< "Failed to deserialize " << "lensControlsSize"
				<< ": not enough data, expected "
				<< (4) << ", got " << (dataSize);
			return ret;
		}
		const size_t lensControlsSize = readPOD<uint32_t>(m, 0, dataEnd);
		m += 4;
		dataSize -= 4;
		if (dataSize < lensControlsSize) {
			LOG(IPADataSerializer, Error)
				<< "Failed to deserialize " << "lensControls"
				<< ": not enough data, expected "
				<< (lensControlsSize) << ", got " << (dataSize);
			return ret;
		}
		if (lensControlsSize > 0)
			ret.lensControls =
				IPADataSerializer<ControlInfoMap>::deserialize(m, m + lensControlsSize, cs);
		m += lensControlsSize;
		dataSize -= lensControlsSize;


		if (dataSize < 4) {
			LOG(IPADataSerializer, Error)
				<< "Failed to deserialize " << "bdsOutputSizeSize"
				<< ": not enough data, expected "
				<< (4) << ", got " << (dataSize);
			return ret;
		}
		const size_t bdsOutputSizeSize = readPOD<uint32_t>(m, 0, dataEnd);
		m += 4;
		dataSize -= 4;
		if (dataSize < bdsOutputSizeSize) {
			LOG(IPADataSerializer, Error)
				<< "Failed to deserialize " << "bdsOutputSize"
				<< ": not enough data, expected "
				<< (bdsOutputSizeSize) << ", got " << (dataSize);
			return ret;
		}
		ret.bdsOutputSize =
			IPADataSerializer<libcamera::Size>::deserialize(m, m + bdsOutputSizeSize, cs);
		m += bdsOutputSizeSize;
		dataSize -= bdsOutputSizeSize;


		if (dataSize < 4) {
			LOG(IPADataSerializer, Error)
				<< "Failed to deserialize " << "iifSize"
				<< ": not enough data, expected "
				<< (4) << ", got " << (dataSize);
			return ret;
		}
		const size_t iifSize = readPOD<uint32_t>(m, 0, dataEnd);
		m += 4;
		dataSize -= 4;
		if (dataSize < iifSize) {
			LOG(IPADataSerializer, Error)
				<< "Failed to deserialize " << "iif"
				<< ": not enough data, expected "
				<< (iifSize) << ", got " << (dataSize);
			return ret;
		}
		ret.iif =
			IPADataSerializer<libcamera::Size>::deserialize(m, m + iifSize, cs);

		return ret;
	}

	static ipa::ipu3::IPAConfigInfo
	deserialize(std::vector<uint8_t> &data,
		    [[maybe_unused]] std::vector<SharedFD> &fds,
		    ControlSerializer *cs = nullptr)
	{
		return IPADataSerializer<ipa::ipu3::IPAConfigInfo>::deserialize(data.cbegin(), data.cend(), cs);
	}

	static ipa::ipu3::IPAConfigInfo
	deserialize(std::vector<uint8_t>::const_iterator dataBegin,
		    std::vector<uint8_t>::const_iterator dataEnd,
		    [[maybe_unused]] std::vector<SharedFD>::const_iterator fdsBegin,
		    [[maybe_unused]] std::vector<SharedFD>::const_iterator fdsEnd,
		    ControlSerializer *cs = nullptr)
	{
		return IPADataSerializer<ipa::ipu3::IPAConfigInfo>::deserialize(dataBegin, dataEnd, cs);
	}
};


} /* namespace libcamera */