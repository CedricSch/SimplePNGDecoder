## SimplePNGDecoder

### Features
- Currently supports only non interlaced images
- Only for Linux
  
### How to install
```
# Build spng
sudo cmake --target spng --build ./build

# Install spng
sudo cmake --install build/
```

### Usage example
```cpp
#include "png.h"
#include "png_utils.h"

int main() {
    PNGData data{readPng("<path>")};
    # image data available with data.imageData
    printPngHeader(data);
}
```
  

