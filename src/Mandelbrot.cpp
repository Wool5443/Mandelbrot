#include <immintrin.h>
#include "Mandelbrot.hpp"
#include "PaletteMaker.hpp"

static const int      SIMULTANEOUS_PIXELS = 8;
static const uint32_t COLOR_INCREMENT     = 0x00110a09;

static const int    N_MAX  = 256;
static const double R2_MAX = 10.f * 10.f;

static const double DX_FACTORS_ARRAY[SIMULTANEOUS_PIXELS] = { 0.f,  1.f,  2.f,  3.f,  4.f,  5.f,  6.f, 7.f };

static const __m512d R2_MAX_512 = _mm512_set1_pd(R2_MAX);
static const __m512d DX_FACTORS = _mm512_set_pd (7.f,  6.f,  5.f,  4.f,  3.f,  2.f,  1.f, 0.f);

ErrorCode DrawMandelbrotNaive(SDL_Surface* surface, Camera* camera, const uint32_t* palette)
{
    MyAssertSoft(surface, ERROR_NULLPTR);
    MyAssertSoft(camera,  ERROR_NULLPTR);

    SDL_LockSurface(surface);

    uint32_t* pixels = (uint32_t*)surface->pixels;

    const double xShift = (double)camera->w / 2.f;
    const double yShift = (double)camera->h / 2.f;

    for (int iy = 0; iy < camera->h; iy++)
    {
        for (int ix = 0; ix < camera->w; ix++)
        {
            const double x0 = ((double)ix - xShift) / camera->scale - camera->x;
            const double y0 = ((double)iy - yShift) / camera->scale - camera->y;

            double x = x0, y = y0;

            int N = 0;

            for (; N < N_MAX; N++)
            {
                const double x2 = x * x;
                const double y2 = y * y;
                const double r2 = x2 + y2;

                if (r2 > R2_MAX)
                    break;

                const double xy = x * y;

                x = x2 - y2 + x0;
                y = xy + xy + y0;
            }

            *(pixels + iy * camera->w + ix) = palette[N % NUMBER_OF_COLORS];
        }
    }

    SDL_UnlockSurface(surface);

    return EVERYTHING_FINE;
}

ErrorCode DrawMandelbrotArrays(SDL_Surface* surface, Camera* camera, const uint32_t* palette)
{
    MyAssertSoft(surface, ERROR_NULLPTR);
    MyAssertSoft(camera,  ERROR_NULLPTR);

    SDL_LockSurface(surface);

    uint32_t* pixels = (uint32_t*)surface->pixels;

    const double xShift = (double)camera->w / 2.f;
    const double yShift = (double)camera->h / 2.f;

    const double revScale = 1 / camera->scale;
    double DX[SIMULTANEOUS_PIXELS] = {};
    for (int i = 0; i < SIMULTANEOUS_PIXELS; i++) DX[i] = revScale * DX_FACTORS_ARRAY[i];

    for (int iy = 0; iy < camera->h; iy++)
    {
        uint32_t* pixelsRow = pixels;

        const double  y0 = ((double)iy - yShift) / camera->scale - camera->y;
        double Y0[SIMULTANEOUS_PIXELS] = {};
        for (int i = 0; i < SIMULTANEOUS_PIXELS; i++) Y0[i] = y0;

        for (int ix = 0; ix < camera->w; ix += SIMULTANEOUS_PIXELS)
        {
            const double  x0 = ((double)ix - xShift) / camera->scale - camera->x;

            double X0[SIMULTANEOUS_PIXELS] = {};
            for (int i = 0; i < SIMULTANEOUS_PIXELS; i++) X0[i] = x0 + DX[i];

            double X[SIMULTANEOUS_PIXELS], Y[SIMULTANEOUS_PIXELS];
            for (int i = 0; i < SIMULTANEOUS_PIXELS; i++) X[i] = X0[i];
            for (int i = 0; i < SIMULTANEOUS_PIXELS; i++) Y[i] = Y0[i];

            uint32_t colors  [SIMULTANEOUS_PIXELS];
            for (int i = 0; i < SIMULTANEOUS_PIXELS; i++) colors[i] = palette[0];

            for (int n = 0; n < N_MAX; n++)
            {
                double X2[SIMULTANEOUS_PIXELS];
                for (int i = 0; i < SIMULTANEOUS_PIXELS; i++) X2[i] = X[i] * X[i];
                double Y2[SIMULTANEOUS_PIXELS];
                for (int i = 0; i < SIMULTANEOUS_PIXELS; i++) Y2[i] = Y[i] * Y[i];
                double XY[SIMULTANEOUS_PIXELS];
                for (int i = 0; i < SIMULTANEOUS_PIXELS; i++) XY[i] = X[i] * Y[i];

                uint8_t cmp[SIMULTANEOUS_PIXELS] = {};
                for (int i = 0; i < SIMULTANEOUS_PIXELS; i++)
                    if (X2[i] + Y2[i] < R2_MAX)
                        cmp[i] = 1;

                if (!*(uint64_t*)cmp) // since cmp is 8 bytes
                    break;

                for (int i = 0; i < SIMULTANEOUS_PIXELS; i++)
                    if (cmp[i])
                        colors[i] = palette[n];
                
                for (int i = 0; i < SIMULTANEOUS_PIXELS; i++)
                    X[i] = X2[i] - Y2[i] + X0[i];
                for (int i = 0; i < SIMULTANEOUS_PIXELS; i++)
                    Y[i] = 2 * XY[i] + Y0[i];
            }

            for (int i = 0; i < SIMULTANEOUS_PIXELS; i++)
                pixelsRow[i] = colors[i];
            pixelsRow += SIMULTANEOUS_PIXELS;
        }
        pixels += camera->w;
    }

    SDL_UnlockSurface(surface);

    return EVERYTHING_FINE;
}

ErrorCode DrawMandelbrotAVX512(SDL_Surface* surface, Camera* camera, const uint32_t* palette)
{
    MyAssertSoft(surface, ERROR_NULLPTR);
    MyAssertSoft(camera,  ERROR_NULLPTR);

    SDL_LockSurface(surface);

    uint32_t* pixels = (uint32_t*)surface->pixels;

    const double xShift = (double)camera->w / 2.f;
    const double yShift = (double)camera->h / 2.f;

    const __m512d DX = _mm512_mul_pd (_mm512_set1_pd(1 / camera->scale), DX_FACTORS);

    for (int iy = 0; iy < camera->h; iy++)
    {
        uint32_t* pixelsRow = pixels;

        const double  y0 = ((double)iy - yShift) / camera->scale - camera->y;
        const __m512d Y0 = _mm512_set1_pd(y0);

        for (int ix = 0; ix < camera->w; ix += SIMULTANEOUS_PIXELS)
        {
            const double  x0 = ((double)ix - xShift) / camera->scale - camera->x;

            const __m512d X0 = _mm512_add_pd(_mm512_set1_pd(x0), DX);

            __m512d X = X0, Y = Y0;

            __m512i  colors        = _mm512_set1_epi32(palette[NUMBER_OF_COLORS - 1]);
            __mmask8 notYetInfinte = 0xFF;

            for (int n = 0; n < N_MAX; n++)
            {
                __m512d X2 = _mm512_mul_pd(X, X);
                __m512d Y2 = _mm512_mul_pd(Y, Y);
                __m512d XY = _mm512_mul_pd(X, Y);

                __mmask8 cmp                 = _mm512_cmplt_pd_mask(_mm512_add_pd(X2, Y2), R2_MAX_512);
                __mmask8 pixelsToChangeColor = cmp ^ notYetInfinte; // if pixel is finite not color
                notYetInfinte               &= cmp;

                colors = _mm512_mask_set1_epi32(colors, pixelsToChangeColor, palette[n]);

                if (!cmp) break;

                X = _mm512_add_pd(_mm512_sub_pd(X2, Y2), X0);
                Y = _mm512_add_pd(_mm512_add_pd(XY, XY), Y0);
            }

            _mm512_storeu_epi32(pixelsRow, colors);
            pixelsRow += SIMULTANEOUS_PIXELS;
        }

        pixels += camera->w;
    }

    SDL_UnlockSurface(surface);

    return EVERYTHING_FINE;
}
