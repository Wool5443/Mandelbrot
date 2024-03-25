#pragma once

#include <SDL2/SDL.h>
#include "Utils.hpp"

#define GET_COORD_TRANSPOSE_PARAMS()                                \
const float dx     = 1.f / 1000.f * camera->scale;                  \
const float dy     = dx;                                            \
const float xShift = (float)camera->w  / 2.f;                       \
const float yShift = (float)camera->h / 2.f                         \

#define TRANSPOSE(x, y)                                             \
x = (x - xShift) * dx - camera->x - X_CENTER_SHIFT;                 \
y = (y - yShift) * dy - camera->y

static const float X_CENTER_SHIFT = 0.5f;

static const float DEFAULT_SCALE = 2.5f;
static const float COORD_STEP    = 0.1f;
static const float SCALE_GROW    = 1.1f;
static const float WHEEL_FACT    = 0.6f;

struct Camera
{
    int   w;
    int   h;

    float x;
    float y;
    float scale;
};

/*
* @brief Draws one frame of Mandelbrot using trivial approach
* @param [in] surface - sdl surface of main window
* @param [in] camera  - camera info
*
* @return error
*/
ErrorCode DrawMandelbrotTrivial(SDL_Surface* surface, Camera* camera);

/*
* @brief Draws one frame of Mandelbrot using trivial approach
* @param [in] surface - sdl surface of main window
* @param [in] camera  - camera info
*
* @return error
*/
ErrorCode DrawMandelbrotAVX512(SDL_Surface* surface, Camera* camera);
