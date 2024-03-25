#include <immintrin.h>
#include "Mandelbrot.hpp"

static int _calculateColors();

static const char  ALPHA = 24;
static const char  BLUE  = 16;
static const char  GREEN =  8;
static const char  RED   =  0;

static const int   N_MAX  = 256;
static const float R2_MAX = 10.f * 10.f;

static const __m512 DX_FACTORS = _mm512_set_ps(0.f, 1.f, 2.f, 3.f, 4.f, 5.f, 6.f, 7.f, 8.f, 9.f, 10.f, 11.f, 12.f, 13.f, 14.f, 15.f);

ErrorCode DrawMandelbrotTrivial(SDL_Surface* surface, Camera* camera)
{
    MyAssertSoft(surface, ERROR_NULLPTR);
    MyAssertSoft(camera, ERROR_NULLPTR);

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

            uint32_t color = 0xff << 24;
            if (N < N_MAX)
            {
                char c = (char)(sqrtf(sqrtf((float)N / (float)N_MAX)) * (float)N_MAX);
                char r = 255 - c;
                char g = (c % 2) * 64;
                char b = c;

                color = (0xff << ALPHA) + (r << RED) + (g << GREEN) + (b << BLUE);
            }
        }
    }

    SDL_UnlockSurface(surface);

    return EVERYTHING_FINE;
}

ErrorCode DrawMandelbrotAVX512(SDL_Surface* surface, Camera* camera)
{

    MyAssertSoft(surface, ERROR_NULLPTR);
    MyAssertSoft(camera, ERROR_NULLPTR);

    SDL_LockSurface(surface);

    uint32_t* pixels = (uint32_t*)surface->pixels;

    GET_COORD_TRANSPOSE_PARAMS();

    const __m512 X_SHIFT  = _mm512_set1_ps(-xShift);
    const __m512 Y_SHIFT  = _mm512_set1_ps(-yShift);
    const __m512 DX       = _mm512_mul_ps(_mm512_set1_ps(dx), DX_FACTORS);
    const __m512 DY       = _mm512_set1_ps(dy);
    const __m512 CAMERA_X = _mm512_set1_ps(-camera->x - X_CENTER_SHIFT);
    const __m512 CAMERA_Y = _mm512_set1_ps(-camera->y);

    for (int iy = 0; iy < camera->h; iy++)
    {
        float x0 = (          - xShift) * dx - camera->x - X_CENTER_SHIFT;
        float y0 = ((float)iy - yShift) * dy - camera->y;

        for (int ix = 0; ix < camera->w; ix += 512 / 32)
        {
            __m512 X0  = _mm512_add_ps(_mm512_mul_ps(X_SHIFT, DX), CAMERA_X);
            __m512 Y0  = _mm512_add_ps(_mm512_mul_ps(_mm512_add_ps(_mm512_set1_ps((float)iy), Y_SHIFT), DY), CAMERA_Y);
            __m512i N = _mm512_setzero_epi32();
        }
    }

    return EVERYTHING_FINE;
}
