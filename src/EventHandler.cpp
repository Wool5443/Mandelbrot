#include "EventHandler.hpp"
#include "PaletteMaker.hpp"

static Drawer _currentDrawer = DEFAULT_DRAWER;

void KeyboardHandler(SDL_Event* e, Camera* camera, bool* running, bool* runBench, DrawFunction_t* drawer,
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
        case SDLK_b:
            *running = false;
            *runBench = true;
            break;
        case SDLK_r:
            _currentDrawer = (Drawer)((_currentDrawer + 1) % NUMBER_OF_DRAWERS);
            *drawer = DRAWERS[_currentDrawer];
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
            camera->x     += COORD_STEP / camera->scale;
            break;
        case SDLK_RIGHT:
        case SDLK_d:
            camera->x     -= COORD_STEP / camera->scale;
            break;
        case SDLK_UP:
        case SDLK_w:
            camera->y     += COORD_STEP / camera->scale;
            break;
        case SDLK_DOWN:
        case SDLK_s:
            camera->y     -= COORD_STEP / camera->scale;
            break;
        default:
            break;
        case SDLK_c:
            camera->scale = DEFAULT_SCALE;
            camera->x     = DEFAULT_CAMERA_X;
            camera->y     = 0.f;
            break;
    }
}

void MouseButtonHandler(SDL_Event* e, Camera* camera, DrawFunction_t* drawer)
{
    MyAssertHard(e,      ERROR_NULLPTR);
    MyAssertHard(camera, ERROR_NULLPTR);

    if (e->button.button == SDL_BUTTON_MIDDLE)
    {
        _currentDrawer = (Drawer)((_currentDrawer + 1) % NUMBER_OF_DRAWERS);
        *drawer = DRAWERS[_currentDrawer];
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

    double xShift = (double)camera->w / 2.;
    double yShift = (double)camera->h / 2.;

    int mouseX = e->wheel.mouseX - xShift;
    int mouseY = e->wheel.mouseY - yShift;

    camera->x += (1 / zoomValue - 1.) * mouseX / camera->scale;
    camera->y += (1 / zoomValue - 1.) * mouseY / camera->scale;

    camera->scale *= zoomValue;
}
