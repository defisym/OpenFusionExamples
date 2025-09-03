#pragma once

#pragma warning(disable : 4244)
#pragma warning(disable : 26495)

#include "zpaq/libzpaq.h"
#include "GeneralDefinition.h"

#ifdef _DEBUG
#pragma	comment(lib, RELATIVE_PATH("zpaqstaticd.lib"))
#else
#pragma	comment(lib, RELATIVE_PATH("zpaqstatic.lib"))
#endif

#include <vector>
#include <string>
#include <memory>
#include <minwindef.h>

constexpr auto ZPAQ_PROCESSFAILED = 0;
//constexpr auto ZPAQ_COMPRESSFAILED = 0;
//constexpr auto ZPAQ_DECOMPRESSFAILED = 1;

using libzpaq::Reader;
using libzpaq::Writer;
using libzpaq::error;
using libzpaq::compress;
using libzpaq::decompress;

inline void libzpaq::error(const char* msg) {
#ifdef _DEBUG
    fprintf(stderr, "Oops: %s\n", msg);
#endif // DEBUG
	
    throw ZPAQ_PROCESSFAILED;
}

class strReader : public libzpaq::Reader {
private:
    std::string srcStr;
    size_t m_offset = 0;

public:
    strReader(const std::string& srcStr) {
        this->srcStr = srcStr;
    }

    int get() {
        if (m_offset < srcStr.length() * sizeof(char)) {
			// must not return -1
            return (unsigned char)(*((char*)srcStr.data() + m_offset++));
        }
        else {
            return -1;
        }
    }
};

class bufReader : public libzpaq::Reader {
private:
    const char* srcBuf;
    int size;
    size_t m_offset = 0;

public:
    bufReader(const char* srcBuf, const int size) {
        this->srcBuf = srcBuf;
        this->size = size;
    }

    int get() {
        if (m_offset < size * sizeof(char)) {
            // must not return -1
            return (unsigned char)(*(srcBuf + m_offset++));
        }
        else {
            return -1;
        }
    }
};

class strWriter : public libzpaq::Writer {
private:
    std::string dstStr;

public:
    strWriter(size_t reserve = 5000) {
        dstStr.reserve(reserve);
    }

    void put(int c) {
        dstStr.push_back(c);
    }

    auto length() {
        return this->dstStr.length();
    }

    const std::string&& output() {
        return std::move(dstStr);
    }
};

class bufWriter : public libzpaq::Writer {
private:
    std::vector<char> dstBuf;

public:
    bufWriter(size_t reserve = 5000) {
        dstBuf.reserve(reserve);
    }

    void put(int c) {
        dstBuf.push_back(c);
    }

    auto size() {
        return this->dstBuf.size();
    }

    void output(char* buf, size_t bufSz) {
        auto sz = (std::min)(dstBuf.size(), bufSz);
        memcpy(buf, dstBuf.data(), sz);
    }
};

//char* buf = nullptr;
//auto compressSz = LZ4I_CompressToBuffer(srcStr, buf);
[[DEPRECATED("Use ZLIBI_Compress instead")]]
inline int ZPAQI_CompressToBuffer(const std::string& srcStr, char*& buf, const char* compressionLevel = "34,128,1") {
    strReader r(srcStr);
    bufWriter w;

    libzpaq::compress(&r, &w, compressionLevel);

    auto compressedSize = w.size();
    buf = new char [compressedSize+1];
    memset(buf, 0, compressedSize + 1);

    w.output(buf, compressedSize);

    return compressedSize;
}

[[DEPRECATED("Use ZLIBI_Compress instead")]]
inline int ZPAQI_CompressToBufferDefault(const std::string& srcStr, char*& buf) {
    return ZPAQI_CompressToBuffer(srcStr, buf);
}

//auto deCompressStr = ZPAQI_DeCompressToString(buf, compressSz);
[[DEPRECATED("Use ZLIBI_Compress instead")]]
inline std::string ZPAQI_DeCompressToString(const char* srcBuf, const unsigned int bufSz, const unsigned char estimateRatio = 0) {
    bufReader r(srcBuf, bufSz);
    strWriter w;

    libzpaq::decompress(&r, &w);

    return w.output();
}

[[DEPRECATED("Use ZLIBI_Compress instead")]]
inline std::string ZPAQI_DeCompressToStringDefault(const char* srcBuf, const unsigned int bufSz) {
    return ZPAQI_DeCompressToString(srcBuf, bufSz);
}