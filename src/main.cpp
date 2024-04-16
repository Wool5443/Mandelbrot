#include <time.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include "PaletteMaker.hpp"
#include "Utils.hpp"
#include "WindowProperties.hpp"
#include "Mandelbrot.hpp"
#include "EventHandler.hpp"
#include "Benchmark.hpp"

static const int ALLIGN_WINDOW_16 = ~0x7;

int main(const int argc, char* argv[])
{
    if (SDL_Init(SDL_INIT_EVERYTHING) != 0)
    {
        fprintf(stderr, "%s\n", fprintf(stderr, "%s\n", SDL_GetError()));
    }
    IMG_Init(IMG_INIT_PNG);

    SDL_Window* window = SDL_CreateWindow(WINDOW_TITLE, 100, 100, WINDOW_WIDTH, WINDOW_HEIGHT,
                                          SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);
    RETURN_ERROR(!window, fprintf(stderr, "%s\n", SDL_GetError()));

    SDL_Event e = {};
    bool running   = true;
    bool mouseDown = false;
    bool runBench  = false;

    DrawFunction_t currentDrawer = DRAWERS[DEFAULT_DRAWER];
    const uint32_t* palette = GetPalette(DEFAULT_PALETTE);

    SDL_Surface* surface = SDL_GetWindowSurface(window);
    SDL_Surface* digits  = IMG_Load(DIGITS_PATH);
    RETURN_ERROR(!surface, fprintf(stderr, "%s\n", SDL_GetError()));
    RETURN_ERROR(!digits, fprintf(stderr, "%s\n", SDL_GetError()));

    Camera camera = 
    {
        .w = surface->w,
        .h = surface->h,
        .x = DEFAULT_CAMERA_X,
        .y = 0,
        .scale = DEFAULT_SCALE,
    };

    timespec start = {};
    timespec end   = {};
    while (running)
    {
        while (SDL_PollEvent(&e))
        {
            switch (e.type)
            {
                case SDL_QUIT:
                    running = false;
                    break;
                case SDL_KEYDOWN:
                    KeyboardHandler(&e, &camera, &running, &runBench, &currentDrawer, &palette);
                    break;
                case SDL_MOUSEBUTTONDOWN:
                    SDL_GetRelativeMouseState(nullptr, nullptr);
                    mouseDown = true;
                    break;
                case SDL_MOUSEBUTTONUP:
                    mouseDown = false;
                    break;
                case SDL_MOUSEWHEEL:
                    MouseWheelHandler(&e, &camera);
                    break;
                case SDL_WINDOWEVENT:
                    if (e.window.event == SDL_WINDOWEVENT_RESIZED)
                    {
                        camera.w = e.window.data1 & ALLIGN_WINDOW_16;
                        camera.h = e.window.data2;
                        printf("w = %d h = %d\n", camera.w, camera.h);

                        SDL_SetWindowSize(window, camera.w, camera.h);
                        surface = SDL_GetWindowSurface(window);
                    }
                    break;
                default:
                    break;
            }
        }

        if (mouseDown)
            MouseButtonHandler(&e, &camera, &currentDrawer);

        uint64_t start = SDL_GetTicks64();
        currentDrawer(surface, &camera, palette);
        uint64_t end   = SDL_GetTicks64();

        ShowFps(surface, digits, (int)(1000. / (double)(end - start)));

        SDL_UpdateWindowSurface(window);
    }

    if (runBench)
    {
        uint64_t ticksNaive  = RunBenchmark(DrawMandelbrotNaive, surface, &camera, (uint32_t*)palette);
        uint64_t ticksArrays = RunBenchmark(DrawMandelbrotArrays, surface, &camera, (uint32_t*)palette);
        uint64_t ticksAVX    = RunBenchmark(DrawMandelbrotAVX512, surface, &camera, (uint32_t*)palette);

        printf("Naive:  %d runs took %llu ticks\n", RUN_TIMES, ticksNaive);
        printf("Arrays: %d runs took %llu ticks\n", RUN_TIMES, ticksArrays);
        printf("AVX:    %d runs took %llu ticks\n", RUN_TIMES, ticksAVX);
        printf("Naive  / Arrays = %.4lg\n", (double)ticksNaive  / (double)ticksArrays);
        printf("Naive  / AVX    = %.4lg\n", (double)ticksNaive  / (double)ticksAVX);
        printf("Arrays / AVX    = %.4lg\n", (double)ticksArrays / (double)ticksAVX);
    }

    SDL_DestroyWindow(window);
    SDL_Quit();
    IMG_Quit();

    return 0;
}
