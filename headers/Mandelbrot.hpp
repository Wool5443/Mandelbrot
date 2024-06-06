#pragma once

#include <SDL2/SDL.h>
#include "Utils.hpp"

static const double DEFAULT_SCALE      = 300.;
static const double DEFAULT_CAMERA_X   = 0.4;
static const double COORD_STEP         = 10.;
static const double SCALE_GROW         = 1.1;
static const double WHEEL_FACT         = 1.1;
static const double REVERSE_WHEEL_FACT = 1 / WHEEL_FACT;

struct Camera
{
    int    w;
    int    h;

    double x;
    double y;
    double scale;
};

/*
* @brief Draws one frame of Mandelbrot using trivial approach
* @param [in] surface - sdl surface of main window
* @param [in] camera  - camera info
*
* @return error
*/
ErrorCode DrawMandelbrotNaive(SDL_Surface* surface, Camera* camera, const uint32_t* palette);


/*
* @brief Draws one frame of Mandelbrot using arrays approach
* @param [in] surface - sdl surface of main window
* @param [in] camera  - camera info
*
* @return error
*/
ErrorCode DrawMandelbrotArrays(SDL_Surface* surface, Camera* camera, const uint32_t* palette);

#ifdef __AVX512__
/*
* @brief Draws one frame of Mandelbrot using trivial approach
* @param [in] surface - sdl surface of main window
* @param [in] camera  - camera info
*
* @return error
*/
ErrorCode DrawMandelbrotAVX512(SDL_Surface* surface, Camera* camera, const uint32_t* palette);
#endif

typedef ErrorCode (*DrawFunction_t)(SDL_Surface*, Camera*, const uint32_t*);
enum Drawer
{
    NAIVE,
    ARRAYS,
#ifdef __AVX512__
    AVX512,
#endif
};

#ifdef __AVX512__
static const int NUMBER_OF_DRAWERS = 3;
static const Drawer         DEFAULT_DRAWER = AVX512;
#else 
static const int NUMBER_OF_DRAWERS = 2;
static const Drawer         DEFAULT_DRAWER = ARRAYS;
#endif
static const DrawFunction_t DRAWERS[] = { DrawMandelbrotNaive, DrawMandelbrotArrays, 
#ifdef __AVX512__
DrawMandelbrotAVX512
#endif 
};
