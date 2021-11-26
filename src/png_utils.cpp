#include "png_utils.h"

uint8_t paethPredictor(uint8_t a, uint8_t b, uint8_t c)
{
    int p = a + b - c;
    int pa = std::abs(p - a);
    int pb = std::abs(p - b);
    int pc = std::abs(p - c);

    if( pa <= pb and pa <= pc) {
      return a;
    } 
    else if( pb <= pc ) {
      return b;
    } 
    else return c;
}

std::ostream& operator<<(std::ostream& out, ColourType colourType) {
    switch(colourType) {

        case ColourType::greyscale: out << "greyscale";
        break;

        case ColourType::greyscale_with_alpha: out << "greyscale_with_alpha";
        break;

        case ColourType::indexed_colour: out << "indexed_colour";
        break;

        case ColourType::truecolour : out << "truecolour";
        break;

        case ColourType::truecolour_with_alpha: out << "truecolour_with_alpha";
        break;

        default: out << "\"" << colourType << "\" unkown colour type.";
        break;
    }

    return out;
}

void printPngHeader(const PNGData& data) {
    std::cout << "Start reading a png file\n";

    std::cout << "\n-------------------------------------------------------------------------------\nImage Header\n-------------------------------------------------------------------------------\n";
    std::cout << "Width: " << data.header.width << "\n";
    std::cout << "Height: " << +data.header.height << "\n";
    std::cout << "Bit Depth: " << +data.header.bitDepth << "\n";
    std::cout << "Colour Type: " << data.header.colourType;
    std::cout << "\n-------------------------------------------------------------------------------\n";
    
    std::cout << "\n-------------------------------------------------------------------------------\nPalette\n-------------------------------------------------------------------------------\n";
    std::cout << "Red: " << +data.plaette.red << "\n";
    std::cout << "Green: " << +data.plaette.green << "\n";
    std::cout << "Blue: " << +data.plaette.blue;
    std::cout << "\n-------------------------------------------------------------------------------\n";

    #ifdef PRINT_IMAGE_DATA_VERBOSE
    std::cout << "\n-------------------------------------------------------------------------------\nInflated data\n-------------------------------------------------------------------------------\n";
    std::for_each(data.imageData.cbegin(), data.imageData.cend(), [](uint8_t b){ std::cout << +b << " "; });
    std::cout << "\n-------------------------------------------------------------------------------\n";
    #endif
}

void flipY(const ImageHeader& header, std::vector<uint8_t>& data) {
  size_t stride = getScanlineWidth(header.width, header.colourType, header.bitDepth) - 1;
  
  for(size_t top = 0, bottom = header.height-1; top < bottom and bottom > top; top++, bottom--) {
    size_t topScanline{top*stride};
    size_t bottomScanline{bottom*stride};

    std::vector<uint8_t> cp(data.begin() + topScanline, data.begin() + topScanline + stride);

    for(int i = 0; i < stride; i++) {
      data[topScanline + i] = data[bottomScanline + i];
    }

    for(int i = 0; i < stride; i++) {
      data[bottomScanline + i] = cp[i];
    }
  }
}