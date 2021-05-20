#ifndef WU_H
#define WU_H

#include "pixel.h"

//------------------------------------------------------------------------------
// Trouve une palette adaptee a l'image avec median cut
// https://en.wikipedia.org/wiki/Median_cut
//------------------------------------------------------------------------------
void guess_palette_wu(IMAGE *image, PALETTE *palette, int palette_size);

#endif
