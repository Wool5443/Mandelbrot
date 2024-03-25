#include "Mandelbrot.hpp"

static int _calculateColors();

static const int   N_MAX  = 256;
static const float R2_MAX = 10.f * 10.f;

static uint32_t* COLORS = (uint32_t*)calloc(N_MAX + 1, sizeof(*COLORS));
static int _dummy = _calculateColors();

static int _calculateColors()
{
    for (int n = 0; n < N_MAX; n++)
    {
        char c = (char)sqrtf(sqrtf((float)n / N_MAX)) * 255.f;

        COLORS[n] = (0xff << 24) | (c << 16) | ((c % 2 * 64) << 8) | (255 - c);
    }

    COLORS[N_MAX] = 0xff << 24; // black

    return 0;
}

ErrorCode DrawMandelbrotTrivial(SDL_Surface* surface, Camera* camera)
{
    MyAssertSoft(surface, ERROR_NULLPTR);

    SDL_LockSurface(surface);

    uint32_t* pixels = (uint32_t*)surface->pixels;

    GET_COORD_TRANSPOSE_PARAMS();

    for (int iy = 0; iy < camera->h; iy++)
    {
        float x0 = (          - xShift) * dx - camera->x - X_CENTER_SHIFT;
        float y0 = ((float)iy - yShift) * dy - camera->y;

        for (int ix = 0; ix < camera->w; ix++, x0 += dx)
        {
            float x = x0, y = y0;

            int N = 0;

            while (N < N_MAX)
            {
                float x2 = x * x;
                float y2 = y * y;
                float r2 = x2 + y2;

                if (r2 > R2_MAX)
                    break;

                float xy = x * y;

                x = x2 - y2 + x0;
                y = xy + xy + y0;

                N++;
            }

            *(pixels + iy * camera->w + ix) = COLORS[N];
        }
    }

    SDL_UnlockSurface(surface);

    return EVERYTHING_FINE;
}
