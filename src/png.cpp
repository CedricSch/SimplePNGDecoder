#include "png.h"
#include "png_utils.h"
#include "zlib_utils.h"

bool readPNGSignature(std::ifstream& stream) {
    uint8_t buffer[8]{};
    stream.read((char*) buffer, 8);

    return buffer[0] == 137 and buffer[1] == 80 and buffer[2] == 78 and buffer[3] == 71 and
           buffer[4] == 13  and buffer[5] == 10 and buffer[6] == 26 and buffer[7] == 10;
}


// TODO: Indexed colours dont work
void reconstructScanlines(const std::vector<uint8_t>& inflatedInput, std::vector<uint8_t>& output, const ImageHeader& header) {
    size_t scanlineWidth{getScanlineWidth(header.width, header.colourType, header.bitDepth)};
    size_t stride{scanlineWidth - 1};
    size_t bytesPerPixel{getBytesPerColourType(header.colourType) * header.bitDepth / 8u };

    for(int currentScanline{0}; currentScanline < header.height; currentScanline++) {
        uint8_t filterType{inflatedInput[currentScanline*scanlineWidth]};

        for(int currentScanlinePosition{0}; currentScanlinePosition < stride; currentScanlinePosition++) 
        {
            size_t nextFilteredBytePosition{currentScanline * scanlineWidth + currentScanlinePosition + 1};

            int x{};
            uint8_t a = ( currentScanlinePosition >= bytesPerPixel                        ) ? output[ (currentScanline       * stride) + currentScanlinePosition - bytesPerPixel] : 0;
            uint8_t b = ( currentScanline > 0                                             ) ? output[ ((currentScanline - 1) * stride) + currentScanlinePosition                ] : 0;
            uint8_t c = ( currentScanline > 0 and currentScanlinePosition >= bytesPerPixel) ? output[ ((currentScanline - 1) * stride) + currentScanlinePosition - bytesPerPixel] : 0;

            switch(filterType) {
                case 0:
                    x = inflatedInput[nextFilteredBytePosition];
                break;

                case 1:
                    x = inflatedInput[nextFilteredBytePosition] + a;
                break;

                case 2:
                    x = inflatedInput[nextFilteredBytePosition] + b;
                break;

                case 3:
                    x = inflatedInput[nextFilteredBytePosition] + ((a + b) / 2);
                break;

                case 4:
                    x = inflatedInput[nextFilteredBytePosition] + paethPredictor(a, b, c);
                break;

                default:
                    std::cerr << "Encountered unsupported filter type: " << +filterType << "\n";
                    throw std::runtime_error("Encountered unsupported filter type.");
            }
            output.push_back(x);
        }
    }
}

void processIHDR(const ChunkInfo& chunk, PNGData& data) {
    uint8_t width[4]{chunk.buffer[0], chunk.buffer[1], chunk.buffer[2], chunk.buffer[3]};
    uint8_t height[4]{chunk.buffer[4], chunk.buffer[5], chunk.buffer[6], chunk.buffer[7]};

    ImageHeader header{};

    header.width = byteArrayToInteger(width);
    header.height = byteArrayToInteger(height); 
    header.bitDepth = chunk.buffer[8];
    header.colourType = ColourType{ chunk.buffer[9] };
    header.compressionMethod = chunk.buffer[10];
    header.filterMethod = chunk.buffer[11];
    header.interlaceMethod = chunk.buffer[12];

    data.header = header;    
}

void processPLTE(const ChunkInfo& chunk, PNGData& data) {
    Palette palette{};

    palette.red = chunk.buffer[0];
    palette.green = chunk.buffer[1];
    palette.blue = chunk.buffer[2];

    data.plaette = palette;
}

// TODO: check crc
void processIDAT(const ChunkInfo& chunk, std::vector<uint8_t>& data) {
    data.insert(data.cend(), chunk.buffer.cbegin(), chunk.buffer.cend());
}

// TODO: check if reads are successfull
ChunkInfo readChunk(std::ifstream& stream) {
    unsigned int length{0};
    unsigned int crc{0};

    uint8_t lengthBuffer[4]{};
    uint8_t crcBuffer[4]{};
    char chunkType[4]{};

    stream.read((char*) lengthBuffer, 4);
    stream.read(chunkType, 4);

    length = byteArrayToInteger(lengthBuffer);
    ChunkInfo chunk{};

    if(length > 0) 
    {
        chunk.buffer.resize(length);
        stream.read((char*) chunk.buffer.data(), length);
        std::cout << "Successfully read chunk data for chunk type \"" << std::string{chunkType} << "\" with length \"" << length << "\"\n";
    }

    stream.read((char*) crcBuffer, 4);
    crc = byteArrayToInteger(crcBuffer);

    chunk.type = chunkType;
    chunk.length = length;
    chunk.crc = crc;

    return chunk;
}

// TODO: Handle interlace
PNGData readPng(const std::string& path) {
    std::ifstream file{path, std::ios_base::binary};
    std::vector<uint8_t> deflatedOutput{};
    
    PNGData pngData{};

    if(not file.is_open()) {
        // TODO: Better error message
        std::cout << "File could not be opened." << std::endl;
        return pngData;
    }

    if (not readPNGSignature(file)) {
        std::cout << "Image has invalid png signature" << std::endl;
        file.close();
    }

    // First chunk needs to be IHDR
    ChunkInfo chunkInfo{readChunk(file)};

    if (chunkInfo.type != "IHDR") {
        std::cout << "First chunk after signature not \"IHDR\".PNG cannot be processed.\n";
        return pngData;
    }

    processIHDR(chunkInfo, pngData);

    do {
        chunkInfo = readChunk(file);
       
        if (chunkInfo.type == "IDAT") {
            processIDAT(chunkInfo, deflatedOutput);
        }
        else if(chunkInfo.type == "PLTE") {
            processPLTE(chunkInfo, pngData);
        }
        else {
            std::cout << "Cant process chunk \"" + chunkInfo.type + "\"\n";
        }
    } while (not file.eof() and chunkInfo.type != "IEND");

    file.close();
    
    try {
        unsigned int scanlineWidth{getScanlineWidth(pngData.header.width, pngData.header.colourType, pngData.header.bitDepth)};
        std::vector<uint8_t> inflatedOutput = decompress(deflatedOutput, pngData.header.height, scanlineWidth);
        reconstructScanlines(inflatedOutput, pngData.imageData, pngData.header);
    } catch(std::exception& ex) {
        std::cerr << ex.what() << "\n";
    }
    return pngData;
}