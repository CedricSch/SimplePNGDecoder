cmake_minimum_required(VERSION 3.16)
project(SimplePNGDecoder VERSION 1.0.0 DESCRIPTION "A simple png decoder" LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(SRC_LIST src/png.cpp src/png_utils.cpp src/zlib_utils.cpp)

enable_testing()

# find zlib package e.g FindZLIB.cmake
find_package(ZLIB REQUIRED)

add_library(spng SHARED ${SRC_LIST})

target_link_libraries(spng ZLIB::ZLIB)
target_include_directories(spng PUBLIC include)

add_subdirectory(test)

install(TARGETS spng LIBRARY DESTINATION /usr/lib)