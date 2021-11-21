#include "zlib_utils.h"
#include "png_exceptions.h"

inline std::string zerrorToString(int ret)
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

// TODO: Only works if image is not interlaced
std::vector<uint8_t> decompress(std::vector<uint8_t>& in, unsigned int height, unsigned int scanlineWidth) {
    unsigned int BUFFER_SIZE = height  * scanlineWidth;

    std::vector<uint8_t> out{};
    out.resize(BUFFER_SIZE);

    unsigned have;
    z_stream strm;

    strm.zalloc = Z_NULL;
    strm.zfree = Z_NULL;
    strm.opaque = Z_NULL;
    strm.avail_in = 0;
    strm.next_in = Z_NULL;
    int ret = inflateInit(&strm);

    if (ret != Z_OK)
        throw PNGDecodeException("Zlib initialization not possible: " + zerrorToString(ret));

    do {
        strm.avail_in = in.size();
        
        if (strm.avail_in == 0)
            break;
        strm.next_in = in.data();

        do {
            strm.avail_out = BUFFER_SIZE;
            strm.next_out = out.data();

            ret = inflate(&strm, Z_NO_FLUSH);
            assert(ret != Z_STREAM_ERROR);  /* state not clobbered */

            switch (ret) {
            case Z_NEED_DICT:
                ret = Z_DATA_ERROR;     /* and fall through */
            case Z_DATA_ERROR:
            case Z_MEM_ERROR:
                (void)inflateEnd(&strm);
                throw PNGDecodeException("Inflation of buffer not possible: " + zerrorToString(ret));
            }
            have = BUFFER_SIZE - strm.avail_out;

        } while(strm.avail_out == 0);
    } while(ret != Z_STREAM_END);

    (void)inflateEnd(&strm);
    
    return out;
}