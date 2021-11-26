#include "png.h"
#include "png_utils.h"
#include "zlib_utils.h"
#include "png_exceptions.h"

bool readPNGSignature(std::ifstream& stream) {
    uint8_t buffer[8]{};
    stream.read((char*) buffer, 8);

    return buffer[0] == 137 and buffer[1] == 80 and buffer[2] == 78 and buffer[3] == 71 and
           buffer[4] == 13  and buffer[5] == 10 and buffer[6] == 26 and buffer[7] == 10;
}

uint8_t applyScanlineFilter(uint8_t filterType, uint8_t unfilteredByte, uint8_t a, uint8_t b, uint8_t c) {
    uint8_t result{};

    switch(filterType) {
                case 0:
                    result = unfilteredByte;
                break;

                case 1:
                    result = unfilteredByte + a;
                break;

                case 2:
                    result = unfilteredByte + b;
                break;

                case 3:
                    result = unfilteredByte + ((a + b) / 2);
                break;

                case 4:
                    result = unfilteredByte + paethPredictor(a, b, c);
                break;

                default:
                    throw PNGDecodeException("Can't reconstruct scanline. Unknown filter type: " + std::to_string(filterType));
    }

    return result;
}

// TODO: Indexed colours dont work
std::vector<uint8_t> reconstructScanlines(const std::vector<uint8_t>& inflatedInput, const ImageHeader& header) {
    size_t scanlineWidth{getScanlineWidth(header.width, header.colourType, header.bitDepth)};
    size_t stride{scanlineWidth - 1};
    size_t bytesPerPixel{getBytesPerColourType(header.colourType) * header.bitDepth / 8u };

    std::vector<uint8_t> output{};

    for(int currentScanline{0}; currentScanline < header.height; currentScanline++) {
        uint8_t filterType{inflatedInput[currentScanline*scanlineWidth]};

        for(int currentScanlinePosition{0}; currentScanlinePosition < stride; currentScanlinePosition++) 
        {
            size_t nextFilteredBytePosition{currentScanline * scanlineWidth + currentScanlinePosition + 1};

            // previous byte
            uint8_t a = ( currentScanlinePosition >= bytesPerPixel                        ) ? output[ (currentScanline       * stride) + currentScanlinePosition - bytesPerPixel] : 0;
            // byte from last scanline
            uint8_t b = ( currentScanline > 0                                             ) ? output[ ((currentScanline - 1) * stride) + currentScanlinePosition                ] : 0;
            // previous byte from last scanline
            uint8_t c = ( currentScanline > 0 and currentScanlinePosition >= bytesPerPixel) ? output[ ((currentScanline - 1) * stride) + currentScanlinePosition - bytesPerPixel] : 0;

            uint8_t result = applyScanlineFilter(filterType, inflatedInput[nextFilteredBytePosition], a, b, c);
        
            output.push_back(result);
        }
    }

    return output;
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
    file.exceptions(std::ios_base::failbit | std::ios_base::badbit);

    std::vector<uint8_t> deflatedOutput{};
    PNGData pngData{};

    if (not readPNGSignature(file))
        throw PNGDecodeException("Invalid png signature.");

    // First chunk needs to be IHDR
    ChunkInfo chunkInfo{readChunk(file)};

    if (chunkInfo.type != "IHDR")
        throw PNGDecodeException("First chunk after signature not from type \"IHDR\".");

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
            std::cerr << "Can't process chunk \"" + chunkInfo.type + "\"\n";
        }
    } while (not file.eof() and chunkInfo.type != "IEND");
  
    unsigned int scanlineWidth{getScanlineWidth(pngData.header.width, pngData.header.colourType, pngData.header.bitDepth)};
    std::vector<uint8_t> inflatedOutput = decompress(deflatedOutput, pngData.header.height, scanlineWidth);
    pngData.imageData = reconstructScanlines(inflatedOutput, pngData.header);

    return pngData;
}