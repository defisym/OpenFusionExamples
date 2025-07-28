#pragma once

#include "zlib/zlib.h"
#include "GeneralDefinition.h"  // for macro RELATIVE_PATH

#ifdef _WIN64
#pragma	comment(lib, RELATIVE_PATH("zlibstatic_x64.lib"))
#else
#pragma	comment(lib, RELATIVE_PATH("zlibstatic.lib"))
#endif

#include <string>

inline bool ZLIBI_Compress(const char* pSrc, const size_t srcSz, 
    std::string& out, const char compressionLevel = Z_DEFAULT_COMPRESSION) {
    const auto sz = compressBound(static_cast<uLong>(srcSz));
    out.clear();
    out.resize(sz, '\0');

    uLong compressedSize = sz;

    const auto ret = compress2(reinterpret_cast<Bytef*>(out.data()),
        &compressedSize,
        reinterpret_cast<const Bytef*>(pSrc),
        static_cast<uLong>(srcSz),
        compressionLevel);

    if (ret != Z_OK) { compressedSize = 0; }
    out.resize(compressedSize);

    return ret == Z_OK;
}

inline bool ZLIBI_Compress(const std::string& in, std::string& out,
    const char compressionLevel = Z_DEFAULT_COMPRESSION) {
    return ZLIBI_Compress(in.data(), in.size(), out, compressionLevel);
}

// return empty string if failed
inline std::string ZLIBI_Compress(const char* pSrc, const size_t srcSz,
    const char compressionLevel = Z_DEFAULT_COMPRESSION) {
    std::string compressed = {};
    ZLIBI_Compress(pSrc, srcSz, compressed, compressionLevel);

    return compressed;
}

inline std::string ZLIBI_Compress(const std::string& in,
    const char compressionLevel = Z_DEFAULT_COMPRESSION) {
    return ZLIBI_Compress(in.data(), in.size(), compressionLevel);
}

// estimateRatio: 
//      0 -> decompress buffer is 8 times larger than input
//      other -> decompress buffer is input size * estimateRatio
inline bool ZLIBI_UnCompress(const char* pSrc, const size_t srcSz,
    std::string& out, const unsigned char estimateRatio = 4) {
    size_t blockSz = !estimateRatio ? (srcSz << 3) : srcSz * estimateRatio;

    while (true) {
        out.clear();
        out.resize(blockSz, '\0');
        auto deCompressedSize = static_cast<uLong>(blockSz);

        const auto ret = uncompress(reinterpret_cast<Bytef*>(out.data()),
            &deCompressedSize,
            reinterpret_cast<const Bytef*>(pSrc),
            static_cast<uLong>(srcSz));

        if (ret == Z_BUF_ERROR) {
            blockSz = blockSz << 1;
            continue;
        }

        if (ret != Z_OK) { deCompressedSize = 0; }
        out.resize(deCompressedSize);

        return ret == Z_OK;
    }

    return false;
}

inline bool ZLIBI_UnCompress(const std::string& in, std::string& out,
    const unsigned char estimateRatio = 4) {
    return ZLIBI_UnCompress(in.data(), in.size(), out, estimateRatio);
}

// return empty string if failed
inline std::string ZLIBI_UnCompress(const char* pSrc, const size_t srcSz,
    const unsigned char estimateRatio = 4) {
    std::string uncompressed = {};
    ZLIBI_UnCompress(pSrc, srcSz, uncompressed, estimateRatio);

    return uncompressed;
}

inline std::string ZLIBI_UnCompress(const std::string& in,
    const unsigned char estimateRatio = 4) {
    return ZLIBI_UnCompress(in.data(), in.size(), estimateRatio);
}
