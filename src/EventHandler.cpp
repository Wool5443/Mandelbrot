#include "EventHandler.hpp"

void KeyboardHandler(SDL_Event* e, Camera* camera, bool* running)
{
    MyAssertHard(e,       ERROR_NULLPTR);
    MyAssertHard(camera,  ERROR_NULLPTR);
    MyAssertHard(running, ERROR_NULLPTR);

    switch (e->key.keysym.sym)
    {
        case SDLK_q:
            *running = false;
            break;
        case SDLK_EQUALS:
        case SDLK_PLUS:
            camera->scale /= SCALE_GROW;
            break;
        case SDLK_MINUS:
            camera->scale *= SCALE_GROW;
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
        case SDLK_c:
            camera->scale = DEFAULT_SCALE;
            camera->x     = 0.f;
            camera->y     = 0.f;
            break;
    }
}

void MouseButtonHandler(SDL_Event* e, Camera* camera)
{
    MyAssertHard(e,      ERROR_NULLPTR);
    MyAssertHard(camera, ERROR_NULLPTR);

    int dx = 0, dy = 0;
    SDL_GetRelativeMouseState(&dx, &dy);

    float dragFact = camera->scale / (float)camera->w;

    camera->x += (float)dx * dragFact;
    camera->y += (float)dy * dragFact;
}

void MouseWheelHandler(SDL_Event* e, Camera* camera)
{
    MyAssertHard(e,      ERROR_NULLPTR);
    MyAssertHard(camera, ERROR_NULLPTR);

    float zoomValue = 1;

    if (e->wheel.y == 1) // zoom in
        zoomValue = WHEEL_FACT;
    else
        zoomValue = 1 / WHEEL_FACT;

    camera->scale *= zoomValue;

    // float x = 0, y = 0;
    // GET_COORD_TRANSPOSE_PARAMS();
    // TRANSPOSE(x, y);

    // camera->x -= x * zoomValue * 0.1f;
    // camera->y -= y * zoomValue * 0.1f;
}
