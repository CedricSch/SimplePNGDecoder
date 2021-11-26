#ifndef PNG_UTILS_H_
#define PNG_UTILS_H_

#include <ranges>
#include "png.h"

// TODO: Is this safe across platforms ? If not convert it into network byte order
inline unsigned int byteArrayToInteger(const uint8_t (&b)[4]) 
{
    unsigned int result{0};

    for(int i = 0; i < 4; i++) {
        result = (result << 8) | b[i];
    }

    return result;
}

inline unsigned getBytesPerColourType(ColourType colourType) {
    switch(colourType) {

        case ColourType::greyscale: return 1;
        break;

        case ColourType::greyscale_with_alpha: return 2;
        break;

        case ColourType::indexed_colour: return 2;
        break;

        case ColourType::truecolour : return 3;
        break;

        case ColourType::truecolour_with_alpha: return 4;
        break;

        default: return 0;
        break;
    }
}

inline unsigned getScanlineWidth(unsigned int width, ColourType colourType, unsigned int bitDepth) {
  return 1 + (width * getBytesPerColourType(colourType) * bitDepth ) / 8;
}

uint8_t paethPredictor(uint8_t, uint8_t, uint8_t);

void printPngHeader(const PNGData&);
void flipY(const ImageHeader&, std::vector<uint8_t>&);

#endif