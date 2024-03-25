#define DEBUG

#include <SDL2/SDL.h>
#include "Utils.hpp"
#include "WindowProperties.hpp"
#include "Mandelbrot.hpp"

static ErrorCode _updateCamera(SDL_Event* e, Camera* camera);

int main()
{
    SDL_Init(SDL_INIT_EVERYTHING);

    SDL_Window* window = SDL_CreateWindow(WINDOW_TITLE, -1, -1, WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_SHOWN);

    SDL_Event e = {};

    SDL_Surface* surface = SDL_GetWindowSurface(window);

    bool running = true;

    Camera camera = 
    {
        .w = surface->w,
        .h = surface->h,
        .x = 0,
        .y = 0,
        .scale = DEFAULT_SCALE_X,
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
                    RETURN_ERROR(_updateCamera(&e, &camera));
            }
        }

        RETURN_ERROR(DrawMandelbrot(surface, &camera));

        SDL_UpdateWindowSurface(window);
    }

    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}

static ErrorCode _updateCamera(SDL_Event* e, Camera* camera)
{
    switch (e->key.keysym.sym)
    {
        case SDLK_EQUALS:
        case SDLK_PLUS:
            camera->scale -= SCALE_GROW;
            break;
        case SDLK_MINUS:
            camera->scale += SCALE_GROW;
            break;
        case SDLK_LEFT:
        case SDLK_a:
            camera->x     += COORD_STEP;
            break;
        case SDLK_RIGHT:
        case SDLK_d:
            camera->x     -= COORD_STEP;
            break;
        case SDLK_UP:
        case SDLK_w:
            camera->y     += COORD_STEP;
            break;
        case SDLK_DOWN:
        case SDLK_s:
            camera->y     -= COORD_STEP;
            break;
        default:
            break;
    }

    return EVERYTHING_FINE;
}
