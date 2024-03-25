#include <SDL2/SDL.h>
#include "Utils.hpp"
#include "WindowProperties.hpp"
#include "Mandelbrot.hpp"
#include "EventHandler.hpp"

int main()
{
    SDL_Init(SDL_INIT_EVERYTHING);

    SDL_Window* window = SDL_CreateWindow(WINDOW_TITLE, -1, -1, WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_SHOWN);

    SDL_Event e = {};

    SDL_Surface* surface = SDL_GetWindowSurface(window);

    bool running   = true;
    bool mouseDown = false;

    Camera camera = 
    {
        .w = surface->w,
        .h = surface->h,
        .x = 0,
        .y = 0,
        .scale = DEFAULT_SCALE,
    };

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
                    KeyboardHandler(&e, &camera, &running);
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
            }
        }

        if (mouseDown)
            MouseButtonHandler(&e, &camera);

        RETURN_ERROR(DrawMandelbrot(surface, &camera));

        SDL_UpdateWindowSurface(window);
    }

    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
