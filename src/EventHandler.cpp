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
            camera->scale *= SCALE_GROW;
            break;
        case SDLK_MINUS:
            camera->scale /= SCALE_GROW;
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

    camera->x += (float)dx / camera->scale;
    camera->y += (float)dy / camera->scale;
}

void MouseWheelHandler(SDL_Event* e, Camera* camera)
{
    MyAssertHard(e,      ERROR_NULLPTR);
    MyAssertHard(camera, ERROR_NULLPTR);

    float zoomValue = 1;

    if (e->wheel.y > 0) // zoom in
        zoomValue = WHEEL_FACT;
    else
        zoomValue = REVERSE_WHEEL_FACT;

    float xShift = (float)camera->w / 2.f;
    float yShift = (float)camera->h / 2.f;

    int mouseX = e->wheel.mouseX - xShift;
    int mouseY = e->wheel.mouseY - yShift;

    camera->x += (1 / zoomValue - 1.f) * mouseX / camera->scale;
    camera->y += (1 / zoomValue - 1.f) * mouseY / camera->scale;

    camera->scale *= zoomValue;
}
