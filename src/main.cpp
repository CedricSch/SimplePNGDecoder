#include <iostream>
#include "png.h"
#include "png_utils.h"

int main() {
    std::cout << "Hello world\n";

    std::string imagePath{"../resources/sample_1920_1080.png"};
    PNGData png{readPng(imagePath)};

    print_png_header(png);
}