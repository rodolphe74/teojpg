#ifndef FLOYD_DITHER
#define FLOYD_DITHER

#include "pixel.h"

static float FS_STANDARD[] = {
	4,
	1, 0,  7.0 / 16,
	-1,1,  3.0 / 16,
	0, 1,  5.0 / 16,
	1, 1,  1.0 / 16
};


static float FS_JARVIS[] = {
	12,
	1, 0,  7.0 / 48,
	2, 0,  5.0 / 48,
	-2,1,  3.0 / 48,
	-1,1,  5.0 / 48,
	0, 1,  7.0 / 48,
	1, 1,  5.0 / 48,
	2, 1,  3.0 / 48,
	-2,2,  1.0 / 48,
	-1,2,  3.0 / 48,
	0, 2,  5.0 / 48,
	1, 2,  3.0 / 48,
	2, 2,  1.0 / 48
};

static float FS_ZHIGANG[] = {
	4,
	1, 0,  7.0 / 16,
	-2,1,  1.0 / 16,
	-1,1,  3.0 / 16,
	0, 1,  5.0 / 16
};

static float FS_SHIAU[] = {
	4,
	1, 0,  1.0 / 2,
	-2,1,  1.0 / 8,
	-1,1,  1.0 / 8,
	0, 1,  1.0 / 4
};

static float FS_SHIAU_2[] = {
	5,
	1, 0,  1.0 / 2,
	-3,1,  1.0 / 16,
	-2,1,  1.0 / 16,
	-1,1,  1.0 / 8,
	0, 1,  1.0 / 4
};

static float FS_STUCKI[] = {
	12,
	1, 0,  4.0 / 21,
	2, 0,  2.0 / 21,
	-2,1,  1.0 / 21,
	-1,1,  2.0 / 21,
	0, 1,  4.0 / 21,
	1, 1,  2.0 / 21,
	2, 1,  1.0 / 21,
	-2,2,  1.0 / 42,
	-1,2,  1.0 / 21,
	0, 2,  2.0 / 21,
	1, 2,  1.0 / 21,
	2, 2,  1.0 / 42
};


static float FS_BURKES[] = {
	7,
	1, 0,  1.0 / 4,
	2, 0,  1.0 / 8,
	-2,1,  1.0 / 16,
	-1,1,  1.0 / 8,
	0, 1,  1.0 / 4,
	1, 1,  1.0 / 8,
	2, 1,  1.0 / 16
};

static float FS_SIERRA[] = {
	10,
	1, 0,  5.0 / 32,
	2, 0,  3.0 / 32,
	-2,1,  1.0 / 16,
	-1,1,  1.0 / 8,
	0, 1,  5.0 / 32,
	1, 1,  1.0 / 8,
	2, 1,  1.0 / 16,
	-1,2,  1.0 / 16,
	0, 2,  3.0 / 32,
	1, 2,  1.0 / 16
};

static float FS_ATKINSON[] = {
	7,
	1, 0,  1.0 / 8,
	2, 0,  1.0 / 8,
	-1,1,  1.0 / 8,
	0, 1,  1.0 / 8,
	1, 1,  1.0 / 8,
	0, 2,  1.0 / 8
};


static float FS_KNUTH[] = {
	8,
	-1,-1,	1.0 / 10,
	0, -1,	1.0 / 5,
	1, -1,	1.0 / 10,
	-1,0,	1.0 / 10,
	1, 0,	1.0 / 10,
	-1,1,	1.0 / 10,
	0, 1,	1.0 / 5,
	1, 1,	1.0 / 10
};


struct FLOYD_MATRIX_STRUCT {
	char	name[64];
	float * matrix;
};
typedef struct FLOYD_MATRIX_STRUCT FLOYD_MATRIX;


static int FLOYD_MATRIX_COUNT = 10;

static FLOYD_MATRIX floyd_matrix[] = {
	{ .name = "Standard", .matrix = FS_STANDARD },
	{ .name = "Jarvis",   .matrix = FS_JARVIS   },
	{ .name = "Zhigang",  .matrix = FS_ZHIGANG  },
	{ .name = "Shiau",    .matrix = FS_SHIAU    },
	{ .name = "Shiau 2",  .matrix = FS_SHIAU_2  },
	{ .name = "Stucki",   .matrix = FS_STUCKI   },
	{ .name = "Burkes",   .matrix = FS_BURKES   },
	{ .name = "Sierra",   .matrix = FS_SIERRA   },
	{ .name = "Atkinson", .matrix = FS_ATKINSON },
	{ .name = "Knuth",    .matrix = FS_KNUTH    }
};



//------------------------------------------------------------------------------
// Floyd Steinberg
// Image retournee doit etre liberee par l'appelant
//------------------------------------------------------------------------------
IMAGE *floyd_steinberg_dither(IMAGE *source, PALETTE *palette, FLOYD_MATRIX *f_mat);


#endif
