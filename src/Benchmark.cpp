#include <SDL2/SDL_image.h>
#include "Benchmark.hpp"

uint64_t RunBenchmark(DrawFunction_t drawFunction, SDL_Surface* surface, Camera* camera, uint32_t* palette)
{
    uint64_t start = GetCPUTicks();
    for (int i = 0; i < RUN_TIMES; i++)
        drawFunction(surface, camera, palette);
    uint64_t end   = GetCPUTicks();

    return end - start;
}

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
