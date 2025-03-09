#include "lz4Interface.h"
#include "zlibInterface.h"
#include "zpaqInterface.h"

#include "data.h"

inline void zpaq() {
	auto srcSz = srcStr.length();

	char* buf = nullptr;
	auto compressSz = LZ4I_CompressToBuffer(srcStr, buf);
	//auto compressSz = ZPAQI_CompressToBuffer(srcStr, buf,"5");

	auto deCompressStr = ZPAQI_DeCompressToString(buf, compressSz);

	auto dstSz = deCompressStr.length();

	//std::cout << deCompressStr << std::endl;

	assert(deCompressStr == srcStr);
}

inline void lz4() {
	auto srcSz = srcStr.length();

	char* buf = nullptr;
	auto compressSz = LZ4I_CompressToBuffer(srcStr, buf);
	//auto compressSz = LZ4I_CompressToBuffer(srcStr, buf, LZ4HC_CLEVEL_MAX);

	auto deCompressStr = LZ4I_DeCompressToString(buf, compressSz);

	auto dstSz = deCompressStr.length();

	//std::cout << deCompressStr << std::endl;

	assert(deCompressStr == srcStr);
}

inline void zlib() {
	auto srcSz = srcStr.length();

	char* buf = nullptr;
	auto compressSz = ZLIBI_CompressToBuffer(srcStr, buf);
	//auto compressSz = ZLIBI_CompressToBuffer(srcStr, buf, Z_BEST_COMPRESSION);

	auto deCompressStr = ZLIBI_DeCompressToString(buf, compressSz);

	auto dstSz = deCompressStr.length();

	//std::cout << deCompressStr << std::endl;

	assert(deCompressStr == srcStr);
}