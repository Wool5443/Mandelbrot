#include <string.h>
#include <math.h>
#include "PaletteMaker.hpp"

static int  _generatePalettes();
static void _generateGrey();
static void _generateBW();
static void _generateGradientRgGbBr();
static void _generateGreyQaattuor();

static uint32_t PALETTES[NUMBER_OF_PALETTES][NUMBER_OF_COLORS] = {};

static int _dummy = _generatePalettes();

static uint currentPalette = DEFAULT_PALETTE;

static void _generateGrey()
{
    for (int i = 0; i < NUMBER_OF_COLORS; i++)
        PALETTES[PALETTE_GREY][i] = (0xff << ALPHA) + (i << RED) + (i << GREEN) + (i << BLUE);
}

static void _generateBW()
{
    memset(PALETTES[PALETTE_BW], 0xff, sizeof(PALETTES[PALETTE_BW]));
    PALETTES[PALETTE_BW][0] = 0xff << ALPHA;
}

static void _generateGradientRgGbBr()
{
    // (255, 0, 0) -> (0, 255, 0)
    for (int n = 0; n < NUMBER_OF_COLORS / 3; n++)
    {
        int i = n;
        PALETTES[PALETTE_GRADIENT_RG_GB_BR][n] = (0xff << ALPHA) + ((NUMBER_OF_COLORS / 3 - i) * 3 << RED) + ((i * 3) << GREEN);
    }
    // (0, 255, 0) -> (0, 0, 255)
    for (int n = NUMBER_OF_COLORS / 3; n < 2 * NUMBER_OF_COLORS / 3; n++)
    {
        int i = n - NUMBER_OF_COLORS / 3;
        PALETTES[PALETTE_GRADIENT_RG_GB_BR][n] = (0xff << ALPHA) + ((NUMBER_OF_COLORS / 3 - i) * 3 << GREEN) + ((i * 3) << BLUE);
    }
    // (0, 0, 255) -> (255, 0, 0)
    for (int n = 2 * NUMBER_OF_COLORS / 3; n < NUMBER_OF_COLORS; n++)
    {
        int i = n - NUMBER_OF_COLORS * 2 / 3;
        PALETTES[PALETTE_GRADIENT_RG_GB_BR][n] = (0xff << ALPHA) + ((NUMBER_OF_COLORS / 3 - i) * 3 << BLUE) + ((i * 3) << RED);
    }
}

static void _generateGreyQaattuor()
{
    for (int i = 0; i < NUMBER_OF_COLORS; i++)
    {
        char c = (char)(sqrt(sqrt((double)i / NUMBER_OF_COLORS)) * (double)NUMBER_OF_COLORS); 
        PALETTES[PALETTE_GREY_QUATTUOR][i] = (0xff << ALPHA) + (c << RED) + (c << GREEN) + (c << BLUE);
    }
}

static int _generatePalettes()
{
    _generateBW();
    _generateGrey();
    _generateGreyQaattuor();
    _generateGradientRgGbBr();

    return 0;
}

const uint32_t* GetPalette(Palettes palette)
{
    return PALETTES[currentPalette];
}

const uint32_t* GetCurrentPalette()
{
    return PALETTES[currentPalette];
}

const uint32_t* GetNextPalette()
{
    currentPalette = (currentPalette + 1) % NUMBER_OF_PALETTES;
    return PALETTES[currentPalette];
}   

const uint32_t* GetPreviousPalette()
{
    currentPalette = (currentPalette - 1) % NUMBER_OF_PALETTES;
    return PALETTES[currentPalette];
}
