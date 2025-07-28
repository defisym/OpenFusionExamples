#pragma once

#include "lz4/lib/lz4hc.h"
#include "GeneralDefinition.h"

#pragma	comment(lib, RELATIVE_PATH("lz4static.lib"))

#include <string>
#include <memory>

constexpr auto LZ4_COMPRESSFAILED = 0;
constexpr auto LZ4_DECOMPRESSFAILED = 1;

//char* buf = nullptr;
//auto compressSz = LZ4I_CompressToBuffer(srcStr, buf);
[[DEPRECATED("Use ZLIBI_Compress instead")]]
inline int LZ4I_CompressToBuffer(const std::string& srcStr, char*& buf , int compressionLevel = LZ4HC_CLEVEL_DEFAULT) {
	auto srcSz = (int)srcStr.length();
	auto sz = LZ4_compressBound(srcSz);	

	buf = new char[sz];
	memset(buf, 0, sz);

	auto compressedSize = LZ4_compress_HC(&srcStr[0], buf, srcStr.length(), sz, compressionLevel);

	if (!compressedSize) {
		throw LZ4_COMPRESSFAILED;
	}

	return compressedSize;
}

[[DEPRECATED("Use ZLIBI_Compress instead")]]
inline int LZ4I_CompressToBufferDefault(const std::string& srcStr, char*& buf) {
	return LZ4I_CompressToBuffer(srcStr, buf);
}

//auto deCompressStr = LZ4I_DeCompressToString(buf, compressSz);
[[DEPRECATED("Use ZLIBI_Compress instead")]]
inline std::string LZ4I_DeCompressToString(const char* srcBuf, const unsigned int bufSz, const unsigned char estimateRatio = 3) {
	const auto blockSz = !estimateRatio ? (bufSz << 8) - bufSz - 2526 : bufSz * estimateRatio;

	std::unique_ptr<char>deComBuf(new char[blockSz + 1]);
	memset(deComBuf.get(), 0, blockSz + 1);

	auto deCompressedSize = LZ4_decompress_safe(srcBuf, deComBuf.get(), bufSz, blockSz);

	if (!deCompressedSize) {
		throw LZ4_DECOMPRESSFAILED;
	}

	auto dstStr = std::string(deComBuf.get(), deCompressedSize);

	return dstStr;
}

[[DEPRECATED("Use ZLIBI_Compress instead")]]
inline std::string LZ4I_DeCompressToStringDefault(const char* srcBuf, const unsigned int bufSz) {
	return LZ4I_DeCompressToString(srcBuf, bufSz);
}