#ifndef PNG_EXCEPTIONS_H_
#define PNG_EXCEPTIONS_H_

#include <string>
#include <stdexcept>

class PNGDecodeException : public std::runtime_error {
public:
    PNGDecodeException(std::string msg) : std::runtime_error(msg) {}


};

#endif