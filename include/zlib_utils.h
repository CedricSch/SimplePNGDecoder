#ifndef ZLIB_UITLS_H_
#define ZLIB_UITLS_H_

#include <zlib.h>
#include <string>
#include <vector>
#include <iostream>

inline std::string zerror_to_string(int ret)
{
    switch (ret) {
    case Z_STREAM_ERROR:
        return std::string{"Z_STREAM_ERROR"};
        break;
    case Z_DATA_ERROR:
        return std::string{"Z_DATA_ERROR"};
        break;
    case Z_MEM_ERROR:
        return std::string{"Z_MEM_ERROR"};
        break;
    case Z_VERSION_ERROR:
        return std::string{"Z_VERSION_ERROR"};
        break;
    default:
        return std::string{"UNKNOWN ERROR"};
        break;
    }
}

std::vector<uint8_t> decompress(std::vector<uint8_t>&, unsigned int, unsigned int);

#endif