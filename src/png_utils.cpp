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

void print_png_header(const PNGData& data) {
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

    std::cout << "\n-------------------------------------------------------------------------------\nInflated data\n-------------------------------------------------------------------------------\n";
    std::for_each(data.imageData.cbegin(), data.imageData.cend(), [](uint8_t b){ std::cout << +b << " "; });
    std::cout << "\n-------------------------------------------------------------------------------\n";
}