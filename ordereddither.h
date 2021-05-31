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

static short BAYER_15_7[] = {
	2,   192, 43,  96,  70,	 58,  128,
	106, 178, 132, 200, 183, 26,  219,
	75,  250, 101, 10,  156, 238, 168,
	161, 55,  29,  241, 120, 108, 19,
	130, 36,  207, 149, 65,	 53,  214,
	87,  226, 152, 77,  224, 38,  202,
	63,  188, 113, 24,  185, 164, 144,
	236, 12,  84,  217, 125, 91,  5,
	137, 123, 248, 171, 14,	 253, 197,
	180, 99,  67,  48,  212, 118, 72,
	41,  176, 221, 159, 135, 79,  51,
	243, 31,  22,  140, 46,	 229, 190,
	115, 209, 166, 111, 103, 34,  154,
	94,  82,  147, 7,   245, 195, 17,
	233, 60,  231, 204, 89,	 173, 142
};
static int BAYER_15_7_SIZE[2] = { 7, 15 };


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
