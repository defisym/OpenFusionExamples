#pragma once

#include "zlib/zlib.h"
#pragma	comment(lib,"zlibstatic.lib")

#include <string>
#include <memory>

constexpr auto ZLIB_COMPRESSFAILED = 0;
constexpr auto ZLIB_DECOMPRESSFAILED = 1;

//char* buf = nullptr;
//auto compressSz = ZLIBI_CompressToBuffer(srcStr, buf);
inline int ZLIBI_CompressToBuffer(const std::string& srcStr, char*& buf, int compressionLevel = Z_DEFAULT_COMPRESSION) {    
    auto srcSz = (uLong)srcStr.length();
    auto sz = compressBound(srcSz);    

    buf = new char [sz];
    memset(buf, 0, sz);

    uLong compressedSize = sz;

    auto ret = compress2((Bytef*)buf, &compressedSize, (Bytef*)&srcStr [0], srcSz, compressionLevel);

    if (ret == Z_OK) {
        return compressedSize;
    }

    throw ZLIB_COMPRESSFAILED;
}

inline int ZLIBI_CompressToBufferDefault(const std::string& srcStr, char*& buf) {
    return ZLIBI_CompressToBuffer(srcStr, buf);
}

//auto deCompressStr = ZLIBI_DeCompressToString(buf, compressSz);
inline std::string ZLIBI_DeCompressToString(const char* srcBuf, const unsigned int bufSz, const unsigned char estimateRatio = 4) {
    uLong blockSz = !estimateRatio ? (bufSz << 3) : bufSz * estimateRatio;

    while (true) {
        std::unique_ptr<char>deComBuf(new char [blockSz + 1]);
        memset(deComBuf.get(), 0, blockSz + 1);

        auto deCompressedSize = blockSz;

        auto ret = uncompress((Bytef*)deComBuf.get(), &deCompressedSize, (Bytef*)srcBuf, bufSz);

        if (ret == Z_OK) {
            auto dstStr = std::string(deComBuf.get(), deCompressedSize);

            return dstStr;
        }

        if (ret == Z_BUF_ERROR) {
            blockSz = blockSz << 1;
            continue;
        }

        throw ZLIB_DECOMPRESSFAILED;
    }
}

inline std::string ZLIBI_DeCompressToStringDefault(const char* srcBuf, const unsigned int bufSz){
    return ZLIBI_DeCompressToString(srcBuf, bufSz);
}