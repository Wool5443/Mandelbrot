#pragma once

#include <SDL2/SDL.h>
#include "Mandelbrot.hpp"

static const char* DIGITS_PATH = "res/numbers2.png";

static const int   DIGIT_WIDTH  = 14;
static const int   DIGIT_HEIGHT = 20;
static const int   FPS_SHIFT_X  = 40;
static const int   FPS_SHIFT_Y  = 20;

static const int RUN_TIMES      = 100;

uint64_t RunBenchmark(DrawFunction_t drawFunction, SDL_Surface* surface, Camera* camera, uint32_t* palette);

void ShowFps(SDL_Surface* surface, SDL_Surface* digits, int fps);
