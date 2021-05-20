#ifndef ORDERED_DITHER
#define ORDERED_DITHER

#include "pixel.h"

struct MIXING_PLAN_STRUCT {
	unsigned char	colors[2];
	float		ratio;
};
typedef struct MIXING_PLAN_STRUCT MIXING_PLAN;

static short BAYER_2_2[2 * 2] = {
	0, 2,
	3, 1
};
static int BAYER_2_2_SIZE[2] = { 2, 2 };

static short BAYER_3_3[3 * 3] = {
	6, 8, 4,
	1, 0, 3,
	5, 2, 7
};
static int BAYER_3_3_SIZE[2] = { 3, 3 };

static short BAYER_4_4[4 * 4] = {
	0,  8,	2,  10,
	12, 4,	14, 6,
	3,  11, 1,  9,
	15, 7,	13, 5
};
static int BAYER_4_4_SIZE[2] = { 4, 4 };

static short BAYER_8_8[8 * 8] = {
	0,  48, 12, 60, 3,  51, 15, 63,
	32, 16, 44, 28, 35, 19, 47, 31,
	8,  56, 4,  52, 11, 59, 7,  55,
	40, 24, 36, 20, 43, 27, 39, 23,
	2,  50, 14, 62, 1,  49, 13, 61,
	34, 18, 46, 30, 33, 17, 45, 29,
	10, 58, 6,  54, 9,  57, 5,  53,
	42, 26, 38, 22, 41, 25, 37, 21
};
static int BAYER_8_8_SIZE[2] = { 8, 8 };

//------------------------------------------------------------------------------
// Dither ordonne
// Retourne la matrice precalcule par rapport a la matrice seuil/bayer
//------------------------------------------------------------------------------
void get_precalculated_matrix(short *matrix, int *matrix_size, float *pre_calc_matrix);

//------------------------------------------------------------------------------
// Dither ordonne
// Image retournee doit etre liberee par l'appelant
//------------------------------------------------------------------------------
IMAGE *ordered_dither(IMAGE *source, PALETTE *palette, float *matrix, int *matrix_size);

//------------------------------------------------------------------------------
// Dither ordonne methode yliluoma
// Image retournee doit etre liberee par l'appelant
//------------------------------------------------------------------------------
IMAGE *ordered_dither_yliluoma(IMAGE *source, PALETTE *palette, float *matrix, int *matrix_size);


#endif
