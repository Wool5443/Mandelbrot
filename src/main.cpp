#include <SDL2/SDL.h>
#include "PaletteMaker.hpp"
#include "Utils.hpp"
#include "WindowProperties.hpp"
#include "Mandelbrot.hpp"
#include "EventHandler.hpp"

static const int ALLIGN_WINDOW_16 = ~0x7;

int main()
{
    SDL_Init(SDL_INIT_EVERYTHING);

    SDL_Window* window = SDL_CreateWindow(WINDOW_TITLE, -1, -1, WINDOW_WIDTH, WINDOW_HEIGHT,
                                          SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);

    SDL_Event e = {};

    SDL_Surface* surface = SDL_GetWindowSurface(window);

    bool running   = true;
    bool mouseDown = false;

    Camera camera = 
    {
        .w = surface->w,
        .h = surface->h,
        .x = DEFAULT_CAMERA_X,
        .y = 0,
        .scale = DEFAULT_SCALE,
    };

    DrawFunction_t currentDrawer = DrawMandelbrotAVX512;

    const uint32_t* palette = GetPalette(DEFAULT_PALETTE);

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
                    KeyboardHandler(&e, &camera, &running, &currentDrawer, &palette);
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

        RETURN_ERROR(currentDrawer(surface, &camera, palette));

        SDL_UpdateWindowSurface(window);
    }

    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
