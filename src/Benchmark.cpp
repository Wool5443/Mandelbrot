#include <SDL2/SDL_image.h>
#include "Benchmark.hpp"

void ShowFps(SDL_Surface* surface, SDL_Surface* digits, int fps)
{
    int x = surface->w - DIGIT_WIDTH - FPS_SHIFT_X;
    int y = DIGIT_HEIGHT + FPS_SHIFT_Y;

    while (fps)
    {
        int digit = fps % 10;
        SDL_Rect dRect = { digit * DIGIT_WIDTH, 0, DIGIT_WIDTH, DIGIT_HEIGHT };
        SDL_Rect sRect = { x, y, DIGIT_HEIGHT, DIGIT_WIDTH };
        SDL_BlitSurface(digits, &dRect, surface, &sRect);

        x -= DIGIT_WIDTH;
        fps /= 10;
    }
}
