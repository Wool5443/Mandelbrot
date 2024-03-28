cmake_minimum_required(VERSION 3.5)
set(projectName "Mandelbrot")

project(${projectName} CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED true)

find_package(SDL2 REQUIRED)
find_package(SDL2_image REQUIRED)
find_package(PkgConfig REQUIRED)
pkg_check_modules(SDL2 REQUIRED sdl2 SDL2_image)

include_directories(${SDL2_INCLUDE_DIRS} ${SDL2_IMAGE_INCLUDE_DIRS})

set(CMAKE_CXX_FLAGS_DEBUG "-march=tigerlake -mavx512f -O0")
set(CMAKE_CXX_FLAGS_RELEASE "-march=tigerlake -mavx512f -O3")

set (SOURCES 
    "Utils/Utils.cpp"
    "Utils/Utils.hpp"
    "src/main.cpp"
    "src/Mandelbrot.cpp"
    "src/EventHandler.cpp"
    "src/PaletteMaker.cpp"
    "src/Benchmark.cpp"
    "headers/EventHandler.hpp"
    "headers/Mandelbrot.hpp"
    "headers/PaletteMaker.hpp"
    "headers/Benchmark.hpp"
)

include_directories(headers Utils)

add_executable(${projectName} ${SOURCES})

target_link_libraries(${projectName} ${SDL2_LIBRARIES} ${SDL2_IMAGE_LIBRARIES})
