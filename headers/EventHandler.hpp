#pragma once

#include "Mandelbrot.hpp"

void KeyboardHandler(SDL_Event* e, Camera* camera, bool* running, DrawFunction_t* currentDrawer);

void MouseButtonHandler(SDL_Event* e, Camera* camera, DrawFunction_t* currentDrawer);

void MouseWheelHandler(SDL_Event* e, Camera* camera);
