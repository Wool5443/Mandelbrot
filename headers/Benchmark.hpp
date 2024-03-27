#pragma once

#include <SDL2/SDL.h>

static const char* DIGITS_PATH = "res/numbers2.png";

static const int   DIGIT_WIDTH  = 14;
static const int   DIGIT_HEIGHT = 20;
static const int   FPS_SHIFT_X  = 40;
static const int   FPS_SHIFT_Y  = 20;

SDL_Surface* InitFPSDigits(const char* digitsPath);

void ShowFps(SDL_Surface* surface, SDL_Surface* digits, int fps);
