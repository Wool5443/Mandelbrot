#pragma once

#include <SDL2/SDL.h>
#include "Utils.hpp"

static const float DEFAULT_SCALE      = 300.f;
static const float DEFAULT_CAMERA_X   = 0.4f;
static const float COORD_STEP         = 0.1f;
static const float SCALE_GROW         = 1.1f;
static const float WHEEL_FACT         = 1.1f;
static const float REVERSE_WHEEL_FACT = 1 / WHEEL_FACT;

struct Camera
{
    int   w;
    int   h;

    double x;
    double y;
    double scale;
};

typedef ErrorCode (*DrawFunction_t)(SDL_Surface*, Camera*, const uint32_t*);

/*
* @brief Draws one frame of Mandelbrot using trivial approach
* @param [in] surface - sdl surface of main window
* @param [in] camera  - camera info
*
* @return error
*/
ErrorCode DrawMandelbrotTrivial(SDL_Surface* surface, Camera* camera, const uint32_t* palette);

/*
* @brief Draws one frame of Mandelbrot using trivial approach
* @param [in] surface - sdl surface of main window
* @param [in] camera  - camera info
*
* @return error
*/
ErrorCode DrawMandelbrotAVX512(SDL_Surface* surface, Camera* camera, const uint32_t* palette);
