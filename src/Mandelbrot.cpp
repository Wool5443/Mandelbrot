#include <immintrin.h>
#include <array>
#include "Mandelbrot.hpp"

inline int _setColors();

static const int      SIMULTANEOUS_PIXELS = 16;
static const uint32_t COLOR_INCREMENT     = 0x00010101;

static const char    ALPHA = 24;
static const char    BLUE  = 16;
static const char    GREEN =  8;
static const char    RED   =  0;

static const int     N_MAX  = 256;
static const float   R2_MAX = 10.f * 10.f;

static const __m512 R2_MAX_512 = _mm512_set1_ps(R2_MAX);
static const __m512 DX_FACTORS = _mm512_set_ps(15.f, 14.f, 13.f, 12.f, 11.f, 10.f,  9.f, 8.f,  
                                                7.f,  6.f,  5.f,  4.f,  3.f,  2.f,  1.f, 0.f);

ErrorCode DrawMandelbrotTrivial(SDL_Surface* surface, Camera* camera)
{
    MyAssertSoft(surface, ERROR_NULLPTR);
    MyAssertSoft(camera, ERROR_NULLPTR);

    SDL_LockSurface(surface);

    uint32_t* pixels = (uint32_t*)surface->pixels;

    const float xShift = (float)camera->w / 2.f;
    const float yShift = (float)camera->h / 2.f;

    for (int iy = 0; iy < camera->h; iy++)
    {
        for (int ix = 0; ix < camera->w; ix++)
        {
            const float x0 = ((float)ix - xShift) / camera->scale - camera->x;
            const float y0 = ((float)iy - yShift) / camera->scale - camera->y;

            float x = x0, y = y0;

            int N = 0;

            while (N < N_MAX)
            {
                const float x2 = x * x;
                const float y2 = y * y;
                const float r2 = x2 + y2;

                if (r2 > R2_MAX)
                    break;

                const float xy = x * y;

                x = x2 - y2 + x0;
                y = xy + xy + y0;

                N++;
            }

            uint32_t color = 0xff << 24;
            if (N < N_MAX)
            {
                char c = (char)(sqrtf(sqrtf((float)N / (float)N_MAX)) * (float)N_MAX);

                char r = c;
                char g = c;
                char b = c;

                color = (0xff << ALPHA) + (r << RED) + (g << GREEN) + (b << BLUE);
            }

            *(pixels + iy * camera->w + ix) = color;
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

    const float xShift = (float)camera->w / 2.f;
    const float yShift = (float)camera->h / 2.f;

    const __m512 X_SHIFT      = _mm512_set1_ps(-xShift);
    const __m512 Y_SHIFT      = _mm512_set1_ps(-yShift);
    const __m512 CAMERA_X     = _mm512_set1_ps(-camera->x);
    const __m512 CAMERA_Y     = _mm512_set1_ps(-camera->y);
    const __m512 REV_SCALE    = _mm512_set1_ps(1 / camera->scale);
    const __m512 REV_SCALE_16 = _mm512_mul_ps(REV_SCALE, _mm512_set1_ps(16.f));
    const __m512 DX           = _mm512_mul_ps (REV_SCALE, DX_FACTORS);

    for (int iy = 0; iy < camera->h; iy++)
    {
        uint32_t* pixelsRow = pixels;
        pixels += camera->w;

        for (int ix = 0; ix < camera->w; ix += SIMULTANEOUS_PIXELS)
        {
            const float  x0 = ((float)ix - xShift) / camera->scale - camera->x;
            const float  y0 = ((float)iy - xShift) / camera->scale - camera->y;

            const __m512 X0 = _mm512_add_ps(_mm512_set1_ps(x0), DX);
            const __m512 Y0 = _mm512_set1_ps(y0);

            __m512 X = X0, Y = Y0;

            __m512i   colors        = _mm512_setzero_epi32();
            uint32_t  currentColor  = 0xff000000;
            __mmask16 notYetInfinte = 0xFFFF;

            for (int n = 0; n < N_MAX; n++)
            {
                __m512 X2 = _mm512_mul_ps(X, X);
                __m512 Y2 = _mm512_mul_ps(Y, Y);
                __m512 XY = _mm512_mul_ps(X, Y);

                __mmask16 cmp                = _mm512_cmplt_ps_mask(_mm512_add_ps(X2, Y2), R2_MAX_512);
                __mmask16 pixelToChangeColor = cmp ^ notYetInfinte; // if pixel is finite not color
                notYetInfinte               &= cmp;

                colors = _mm512_mask_set1_epi32(colors, pixelToChangeColor, currentColor);

                // if (!cmp) break;

                X = _mm512_add_ps(_mm512_sub_ps(X2, Y2), X0);
                Y = _mm512_add_ps(_mm512_add_ps(XY, XY), Y0);

                currentColor += COLOR_INCREMENT;
            }

            _mm512_storeu_epi32(pixelsRow, colors);
            pixelsRow += SIMULTANEOUS_PIXELS;
        }
    }

    SDL_UnlockSurface(surface);

    return EVERYTHING_FINE;
}
