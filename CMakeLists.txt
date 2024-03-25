cmake_minimum_required(VERSION 3.5)

project("Mandelbrot")

find_package(SDL2 REQUIRED)

include_directories(${SDL2_INCLUDE_DIRS})

set(CMAKE_CXX_FLAGS_DEBUG
"-Wno-switch -Wno-conversion -Wno-unused-variable -Wno-pointer-arith -ggdb3 -Wall                                           \
-Wextra -Weffc++ -Waggressive-loop-optimizations -Wc++14-compat -Wmissing-declarations -Wcast-align -Wchar-subscripts       \
-Wconditionally-supported -Wctor-dtor-privacy -Wempty-body -Wformat-nonliteral -Wformat-security -Wformat-signedness        \
-Wformat=2 -Winline -Wlogical-op -Wnon-virtual-dtor -Wopenmp-simd -Woverloaded-virtual -Wpacked -Winit-self                 \
-Wredundant-decls -Wsign-promo -Wstrict-null-sentinel -Wstrict-overflow=2 -Wsuggest-attribute=noreturn                      \
-Wsuggest-final-methods -Wsuggest-final-types -Wsuggest-override -Wswitch-default -Wsync-nand -Wundef -Wunreachable-code    \
-Wunused -Wuseless-cast -Wvariadic-macros -Wno-literal-suffix -Wno-missing-field-initializers -Wno-narrowing                \
-Wno-old-style-cast -Wno-varargs -Wstack-protector -fcheck-new -fsized-deallocation -fstack-protector -fstrict-overflow     \
-flto-odr-type-merging -fno-omit-frame-pointer -pie -fPIE -Werror=vla -fsanitize=address,alignment,bool,bounds,enum,        \
float-cast-overflow,float-divide-by-zero,integer-divide-by-zero,leak,nonnull-attribute,null,object-size,return,             \
returns-nonnull-attribute,shift,signed-integer-overflow,undefined,unreachable,vla-bound,vptr -S")

set(CMAKE_CXX_FLAGS_RELEASE "-Wall -Wno-switch -Wno-conversion -Wno-unused-variable -Wno-pointer-arith")

set(EXECUTABLE "main")

add_definitions("-DDEBUG")

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
