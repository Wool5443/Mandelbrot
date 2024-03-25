#pragma once

#include <SDL2/SDL.h>
#include "Utils.hpp"

static const float DEFAULT_SCALE_X = 2.f;
static const float COORD_STEP      = 0.1f;
static const float SCALE_GROW      = 0.75;

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
