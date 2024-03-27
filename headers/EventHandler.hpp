#pragma once

#include "Mandelbrot.hpp"

void KeyboardHandler(SDL_Event* e, Camera* camera, bool* running, bool* runBench, DrawFunction_t* currentDrawer,
                    const uint32_t** palette);

void MouseButtonHandler(SDL_Event* e, Camera* camera, DrawFunction_t* currentDrawer);

void MouseWheelHandler(SDL_Event* e, Camera* camera);
