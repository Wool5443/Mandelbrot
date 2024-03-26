#include <SDL2/SDL.h>
#include "Utils.hpp"
#include "WindowProperties.hpp"
#include "Mandelbrot.hpp"
#include "EventHandler.hpp"

static const uint8_t ALLIGN_WINDOW_16 = 0xF;

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

    // ErrorCode (*currentDrawer)(SDL_Surface*, Camera*) = DrawMandelbrotAVX512;
    DrawFunction_t currentDrawer = DrawMandelbrotAVX512;

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
                    KeyboardHandler(&e, &camera, &running, &currentDrawer);
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
                        surface = SDL_GetWindowSurface(window);
                        camera.w = surface->w & ALLIGN_WINDOW_16;
                        camera.h = surface->h & ALLIGN_WINDOW_16;
                    }
                    break;
                default:
                    break;
            }
        }

        if (mouseDown)
            MouseButtonHandler(&e, &camera, &currentDrawer);

        RETURN_ERROR(currentDrawer(surface, &camera));

        SDL_UpdateWindowSurface(window);
    }

    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
