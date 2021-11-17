cmake_minimum_required(VERSION 3.16)
project(SimplePNGDecoder VERSION 1.0.0)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

set(SRC_LIST src/main.cpp src/png.cpp src/png_utils.cpp src/zlib_utils.cpp)

# find zlib package e.g FindZLIB.cmake
find_package(ZLIB REQUIRED)

add_executable(main ${SRC_LIST})

target_link_libraries(main ZLIB::ZLIB)
target_include_directories(main PUBLIC include)