cmake_minimum_required(VERSION 3.5)

project("Mandelbrot")

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED true)

find_package(SDL2 REQUIRED)

include_directories(${SDL2_INCLUDE_DIRS})

set(CMAKE_CXX_FLAGS_RELEASE "-march=tigerlake -mavx512f -O3")

set(EXECUTABLE "main")

set (SOURCES 
    "Utils/Utils.cpp"
    "Utils/Utils.hpp"
    "src/main.cpp"
    "src/Mandelbrot.cpp"
    "src/EventHandler.cpp"
    "headers/EventHandler.hpp"
    "headers/Mandelbrot.hpp"
)

include_directories(headers Utils)

add_executable(${EXECUTABLE} ${SOURCES})

target_link_libraries(${EXECUTABLE} ${SDL2_LIBRARIES})
