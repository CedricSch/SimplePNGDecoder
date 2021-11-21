#ifndef ZLIB_UITLS_H_
#define ZLIB_UITLS_H_

#include <zlib.h>
#include <string>
#include <vector>
#include <iostream>
#include <cassert>

inline std::string zerrorToString(int);
std::vector<uint8_t> decompress(std::vector<uint8_t>&, unsigned int, unsigned int);

#endif