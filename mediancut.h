#ifndef MEDIANCUT_H
#define MEDIANCUT_H

#include "pixel.h"

struct BUCKET_STRUCT {
	unsigned char **colors;
	unsigned int	size;
	unsigned char	mean[3];
};
typedef struct BUCKET_STRUCT BUCKET;


//------------------------------------------------------------------------------
// Trouve une palette adaptee a l'image avec median cut
// https://en.wikipedia.org/wiki/Median_cut
//------------------------------------------------------------------------------
void guess_palette_median_cut(IMAGE *image, PALETTE *palette, int iter);

//------------------------------------------------------------------------------
// TODO reecrire sans BUCKET
//------------------------------------------------------------------------------
void find_threshold(PALETTE * palette, unsigned char threshold[3]);

#endif
