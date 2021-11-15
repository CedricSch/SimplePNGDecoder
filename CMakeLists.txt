cmake_minimum_required(VERSION 3.16)
project(SimplePNGDecoder VERSION 1.0.0)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# TODO: dont specify every source file
add_executable(main src/main.cpp src/png.cpp src/png_utils.cpp src/zlib_utils.cpp)

target_link_options(main PRIVATE "LINKER:SHELL:--no-as-needed -lz")
target_include_directories(main PUBLIC include /usr/local/include/glfw-3.3.4 /usr/local/include/glad /usr/local/include/KHR)
target_link_libraries(main /usr/local/lib/glfw-3.3.4/libglfw3.a)
