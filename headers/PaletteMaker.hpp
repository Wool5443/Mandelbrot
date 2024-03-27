#pragma once
#include "Utils.hpp"

enum Palettes
{
    PALETTE_BW,
    PALETTE_GREY,
    PALETTE_GREY_QUATTUOR,
    PALETTE_GRADIENT_RG_GB_BR,
};

static const char ALPHA = 24;
static const char BLUE  = 16;
static const char GREEN =  8;
static const char RED   =  0;

static const int NUMBER_OF_COLORS   = 256;
static const int NUMBER_OF_PALETTES = 4;

static const Palettes DEFAULT_PALETTE = PALETTE_GREY;

const uint32_t* GetPalette(Palettes palette);
const uint32_t* GetCurrentPalette();
const uint32_t* GetNextPalette();
const uint32_t* GetPreviousPalette();
