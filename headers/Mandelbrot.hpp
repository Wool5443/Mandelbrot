#pragma once

#include <SDL2/SDL.h>
#include "Utils.hpp"

#define GET_COORD_TRANSPOSE_PARAMS()                                \
const float dx     = 1.f / (float)camera->w * camera->scale;        \
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
* @brief Draws one frame of Mandelbrot
* @param [in] surface - sdl surface of main window
* @param [in] scale  - inverse scale factor, aka scale = 2 is scaling outwards
* @param [in] scale  - inverse scale factor, aka scale = 2 is scaling outwards
*
* @return error
*/
ErrorCode DrawMandelbrot(SDL_Surface* surface, Camera* camera);
