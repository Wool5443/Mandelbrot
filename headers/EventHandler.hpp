#pragma once

#include "Mandelbrot.hpp"

void KeyboardHandler(SDL_Event* e, Camera* camera, bool* running);

void MouseButtonHandler(SDL_Event* e, Camera* camera);

void MouseWheelHandler(SDL_Event* e, Camera* camera);
