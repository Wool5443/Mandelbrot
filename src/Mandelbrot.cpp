#include <immintrin.h>
#include <array>
#include "Mandelbrot.hpp"

static const int      SIMULTANEOUS_PIXELS = 8;
static const uint32_t COLOR_INCREMENT     = 0x00110a09;

static const char ALPHA = 24;
static const char BLUE  = 16;
static const char GREEN =  8;
static const char RED   =  0;

static const int    N_MAX  = 256;
static const double R2_MAX = 10.f * 10.f;

static const __m512d R2_MAX_512 = _mm512_set1_pd(R2_MAX);
static const __m512d DX_FACTORS = _mm512_set_pd (7.f,  6.f,  5.f,  4.f,  3.f,  2.f,  1.f, 0.f);

ErrorCode DrawMandelbrotTrivial(SDL_Surface* surface, Camera* camera, const uint32_t* palette)
{
    MyAssertSoft(surface, ERROR_NULLPTR);
    MyAssertSoft(camera, ERROR_NULLPTR);

    SDL_LockSurface(surface);

    uint32_t* pixels = (uint32_t*)surface->pixels;

    const float xShift = (double)camera->w / 2.f;
    const float yShift = (double)camera->h / 2.f;

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

            *(pixels + iy * camera->w + ix) = palette[N];
        }
    }

    SDL_UnlockSurface(surface);

    return EVERYTHING_FINE;
}

ErrorCode DrawMandelbrotAVX512(SDL_Surface* surface, Camera* camera, const uint32_t* palette)
{
    MyAssertSoft(surface, ERROR_NULLPTR);
    MyAssertSoft(camera, ERROR_NULLPTR);

    SDL_LockSurface(surface);

    uint32_t* pixels = (uint32_t*)surface->pixels;

    const double xShift = (double)camera->w / 2.f;
    const double yShift = (double)camera->h / 2.f;

    const __m512d X_SHIFT   = _mm512_set1_pd(-xShift);
    const __m512d Y_SHIFT   = _mm512_set1_pd(-yShift);
    const __m512d CAMERA_X  = _mm512_set1_pd(-camera->x);
    const __m512d CAMERA_Y  = _mm512_set1_pd(-camera->y);
    const __m512d REV_SCALE = _mm512_set1_pd(1 / camera->scale);
    const __m512d DX        = _mm512_mul_pd (REV_SCALE, DX_FACTORS);

    for (int iy = 0; iy < camera->h; iy++)
    {
        uint32_t* pixelsRow = pixels;
        pixels += camera->w;

        for (int ix = 0; ix < camera->w; ix += SIMULTANEOUS_PIXELS)
        {
            const double  x0 = ((double)ix - xShift) / camera->scale - camera->x;
            const double  y0 = ((double)iy - yShift) / camera->scale - camera->y;

            const __m512d X0 = _mm512_add_pd(_mm512_set1_pd(x0), DX);
            const __m512d Y0 = _mm512_set1_pd(y0);

            __m512d X = X0, Y = Y0;

            __m512i   colors        = _mm512_set1_epi32(palette[0]);
            __mmask16 notYetInfinte = 0xFFFF;

            for (int n = 0; n < N_MAX; n++)
            {
                __m512d X2 = _mm512_mul_pd(X, X);
                __m512d Y2 = _mm512_mul_pd(Y, Y);
                __m512d XY = _mm512_mul_pd(X, Y);

                __mmask16 cmp                = _mm512_cmplt_pd_mask(_mm512_add_pd(X2, Y2), R2_MAX_512);
                __mmask16 pixelToChangeColor = cmp ^ notYetInfinte; // if pixel is finite not color
                notYetInfinte               &= cmp;

                colors = _mm512_mask_set1_epi32(colors, pixelToChangeColor, palette[n]);

                if (!cmp) break;

                X = _mm512_add_pd(_mm512_sub_pd(X2, Y2), X0);
                Y = _mm512_add_pd(_mm512_add_pd(XY, XY), Y0);
            }

            _mm512_storeu_epi32(pixelsRow, colors);
            pixelsRow += SIMULTANEOUS_PIXELS;
        }
    }

    SDL_UnlockSurface(surface);

    return EVERYTHING_FINE;
}
