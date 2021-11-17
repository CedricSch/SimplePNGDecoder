#ifndef PNG_H_
#define PNG_H_

#include <iostream>
#include <string>
#include <fstream>
#include <memory>
#include <vector>
#include <algorithm>

enum class ColourType {
    greyscale = 0,
    truecolour = 2,
    indexed_colour = 3,
    greyscale_with_alpha = 4,
    truecolour_with_alpha = 6
};

std::ostream& operator<<(std::ostream&, ColourType);

// IHDR
struct ImageHeader {
    unsigned int width;
    unsigned int height;
    // bits per sample or per palette index. valid values are 1, 2, 4, 8, 16
    unsigned char bitDepth;
    // defines the png image type. valid values are 0, 2, 3, 4, 6
    ColourType colourType;
    uint8_t compressionMethod;
    uint8_t filterMethod;
    uint8_t interlaceMethod;
};

// PLTE
struct Palette {
    uint8_t red;
    uint8_t green;
    uint8_t blue;
};

// TODO: Replace unique_ptr with vector
struct ChunkInfo {
    ChunkInfo() : ChunkInfo("UNKNOWN", 0, 0) {}
    ChunkInfo(std::string_view chunkType, unsigned int dataLength, unsigned int crc) : 
              type{chunkType},length{dataLength}, crc{crc} 
    {}

    std::string type;
    std::vector<uint8_t> buffer;
    unsigned int length;
    unsigned int crc;
};

struct PNGData {
    ImageHeader header;
    Palette plaette;
    std::vector<uint8_t> imageData;
    bool isValid = false;
};

PNGData readPng(const std::string&);

#endif