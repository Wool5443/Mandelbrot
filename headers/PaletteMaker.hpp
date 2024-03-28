#pragma once
#include "Utils.hpp"

enum Palettes
{
    PALETTE_BW,
    PALETTE_GREY,
    PALETTE_GREY_QUATTUOR,
    PALETTE_GRADIENT_RG_GB_BR,
    PALETTE_GRADIENT_RG,
};

static const char ALPHA = 24;
static const char BLUE  =  0;
static const char GREEN =  8;
static const char RED   = 16;

static const int NUMBER_OF_COLORS   = 256;
static const int NUMBER_OF_PALETTES = 5;

static const Palettes DEFAULT_PALETTE = PALETTE_GREY;

const uint32_t* GetPalette(Palettes palette);
const uint32_t* GetCurrentPalette();
const uint32_t* GetNextPalette();
const uint32_t* GetPreviousPalette();
