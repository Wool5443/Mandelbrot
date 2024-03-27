#include "EventHandler.hpp"
#include "PaletteMaker.hpp"

void KeyboardHandler(SDL_Event* e, Camera* camera, bool* running, DrawFunction_t* currentDrawer,
                    const uint32_t** palette)
{
    MyAssertHard(e,       ERROR_NULLPTR);
    MyAssertHard(camera,  ERROR_NULLPTR);
    MyAssertHard(running, ERROR_NULLPTR);

    switch (e->key.keysym.sym)
    {
        case SDLK_x:
            *running = false;
            break;
        case SDLK_r:
            if (*currentDrawer == DrawMandelbrotAVX512)
                *currentDrawer = DrawMandelbrotTrivial;
            else
                *currentDrawer = DrawMandelbrotAVX512;
            break;
        case SDLK_e:
            *palette = GetNextPalette();
            break;
        case SDLK_q:
            *palette = GetPreviousPalette();
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

void MouseButtonHandler(SDL_Event* e, Camera* camera, DrawFunction_t* currentDrawer)
{
    MyAssertHard(e,      ERROR_NULLPTR);
    MyAssertHard(camera, ERROR_NULLPTR);

    if (e->button.button == SDL_BUTTON_MIDDLE)
    {
       if (*currentDrawer == DrawMandelbrotAVX512)
            *currentDrawer = DrawMandelbrotTrivial;
        else
            *currentDrawer = DrawMandelbrotAVX512; 
        return;
    }

    int dx = 0, dy = 0;
    SDL_GetRelativeMouseState(&dx, &dy);

    camera->x += (double)dx / camera->scale;
    camera->y += (double)dy / camera->scale;
}

void MouseWheelHandler(SDL_Event* e, Camera* camera)
{
    MyAssertHard(e,      ERROR_NULLPTR);
    MyAssertHard(camera, ERROR_NULLPTR);

    double zoomValue = 1;

    if (e->wheel.y > 0) // zoom in
        zoomValue = WHEEL_FACT;
    else
        zoomValue = REVERSE_WHEEL_FACT;

    double xShift = (double)camera->w / 2.f;
    double yShift = (double)camera->h / 2.f;

    int mouseX = e->wheel.mouseX - xShift;
    int mouseY = e->wheel.mouseY - yShift;

    camera->x += (1 / zoomValue - 1.f) * mouseX / camera->scale;
    camera->y += (1 / zoomValue - 1.f) * mouseY / camera->scale;

    camera->scale *= zoomValue;
}
