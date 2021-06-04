#ifndef PIXEL_H
#define PIXEL_H

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>


struct PIXEL_STRUCT {
	unsigned char	r;
	unsigned char	g;
	unsigned char	b;
};
typedef struct PIXEL_STRUCT PIXEL;



struct IMAGE_STRUCT {
	unsigned short	width;
	unsigned short	height;
	PIXEL *		pixels;
};
typedef struct IMAGE_STRUCT IMAGE;


struct PIXEL_STRUCT_SHORT {
	short	r;
	short	g;
	short	b;
};
typedef struct PIXEL_STRUCT_SHORT PIXEL_SHORT;


struct PALETTE_STRUCT {
	unsigned char	colors[32][3];
	unsigned char	size;
	char		name[50];
};
typedef struct PALETTE_STRUCT PALETTE;


struct PALETTE_IMAGE_STRUCT {
	PALETTE *	palette;
	unsigned char * pixels;
};
typedef struct PALETTE_IMAGE_STRUCT PALETTE_IMAGE;


typedef unsigned char RGB_COLOR[3];
typedef unsigned char INDEXED_COLOR;



//------------------------------------------------------------------------------
// Conversion rgb<->cie linéaire
//------------------------------------------------------------------------------
short linear_space(short x);
float linear_space_f(float x);
short rgb_space(short x);
float rgb_space_f(float x);



//------------------------------------------------------------------------------
// Chargement d'une palette
//------------------------------------------------------------------------------
void load_palette(PALETTE *palette, const int colors[][3], int size);

//------------------------------------------------------------------------------
// Création d'une palette orthogonale
//------------------------------------------------------------------------------
void generate_eq_space_palette(PALETTE *palette, int size);

//------------------------------------------------------------------------------
// Conversion palette
//------------------------------------------------------------------------------
void convert_linear_palette_to_rgb(PALETTE *palette);
void convert_rgb_palette_to_linear(PALETTE *palette);


//------------------------------------------------------------------------------
// Trouve une palette adaptee a l'image
//------------------------------------------------------------------------------
void guess_palette(IMAGE *image, PALETTE *palette);

//------------------------------------------------------------------------------
// Distance entre 2 couleurs
//------------------------------------------------------------------------------
float color_delta_f(unsigned char c1[3], unsigned char c2[3]);

//------------------------------------------------------------------------------
// Compte le nombre de couleurs de l'image et
// retourne la palette max 32 couleurs
//------------------------------------------------------------------------------
int count_colors_32(IMAGE *image, PALETTE *palette);


//------------------------------------------------------------------------------
// Retrouve la couleur la plus proche par rapport la palette
// Retourne la couleur RGB dans le paramètre color
//------------------------------------------------------------------------------
void find_closest_color_rgb(PIXEL *pixel, PALETTE *palette, RGB_COLOR *color);

//------------------------------------------------------------------------------
// Retrouve la couleur la plus proche par rapport la palette
// Retourne l'index de couleur dans la palette
//------------------------------------------------------------------------------
int find_closest_color_index(PIXEL *pixel, PALETTE *palette);


//------------------------------------------------------------------------------
// Retourne la couleur complémentaire
//------------------------------------------------------------------------------
void complementary_color(RGB_COLOR source, RGB_COLOR *target);


//------------------------------------------------------------------------------
// Convertit un pixel codé sur 16 bits en 8 bits
//------------------------------------------------------------------------------
void convert_pixel(PIXEL_SHORT *source, PIXEL *target);

//------------------------------------------------------------------------------
// Creation struct IMAGE à partir d'un tableau de pixels
//------------------------------------------------------------------------------
IMAGE *create_image(unsigned char *pixels, PALETTE *palette, int w, int h);

//------------------------------------------------------------------------------
// Creation struct IMAGE vide
//------------------------------------------------------------------------------
IMAGE *create_empty_image(int w, int h);

//------------------------------------------------------------------------------
// Conversion images
//------------------------------------------------------------------------------
IMAGE *convert_rgb_image_to_linear(IMAGE *image);
IMAGE *convert_linear_image_to_rgb(IMAGE *image);


//------------------------------------------------------------------------------
// Creation d'un tableau de pixels à partir d'une struct IMAGE
//------------------------------------------------------------------------------
unsigned char *create_pixels_array(IMAGE *image, PALETTE *palette);


//------------------------------------------------------------------------------
// Redimensionnement de l'image
//------------------------------------------------------------------------------
IMAGE *bilinear_resize(IMAGE *image, unsigned short width, unsigned short height);

//------------------------------------------------------------------------------
// gamma de l'image
//------------------------------------------------------------------------------
IMAGE *gamma(IMAGE *image, float gamma);

//------------------------------------------------------------------------------
// luminosite et contraste de l'image
//------------------------------------------------------------------------------
IMAGE *brightness_and_contrast(IMAGE *image, float brightness, float contrast);

//------------------------------------------------------------------------------
// saturation des couleurs
//------------------------------------------------------------------------------
IMAGE *saturation(IMAGE *image, float sat);


//------------------------------------------------------------------------------
// Convertit composantes 4 bits vers 8 bits
//------------------------------------------------------------------------------
void _16_to_256_comp(int r_16, int g_16, int b_16, int *r_256, int *g_256, int *b_256);

//------------------------------------------------------------------------------
// Convertit composantes 8 bits vers 4 bits
//------------------------------------------------------------------------------
void _256_to_16_comp(int r_256, int g_256, int b_256, int *r_16, int *g_16, int *b_16);

//------------------------------------------------------------------------------
// Chargement d'une palette prédefinie format hex
// Retourne vrai si ok
//------------------------------------------------------------------------------
int load_hex_palette(char *filename, PALETTE *palette);

#endif
