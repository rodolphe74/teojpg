#include "pixel.h"
// TODO threshold
#include "mediancut.h"
#include "log.h"
#include "maputil.h"
#include "colourconverter.h"
#include <stdio.h>
#include <string.h>
#include <limits.h>
#include <math.h>
#include <map.h>
#include <float.h>


#define max(a, b) (((a) > (b)) ? (a) : (b))
#define min(a, b) (((a) < (b)) ? (a) : (b))

#define min_f(a, b, c)  (fminf(a, fminf(b, c)))
#define max_f(a, b, c)  (fmaxf(a, fmaxf(b, c)))


unsigned short flr(double x)
{
	return (unsigned short)x - (x < (unsigned short)x);
}



unsigned short cl(double x)
{
	return (unsigned short)x + (x > (unsigned short)x);
}



void _16_to_256_comp(int r_16, int g_16, int b_16, int *r_256, int *g_256, int *b_256)
{
	/*
	 * r_256 = (r_16 + 1) * 16 - 1;
	 * g_256 = (g_16 + 1) * 16 - 1;
	 * b_256 = (b_16 + 1) * 16 - 1;
	 */
	*r_256 = r_16 * 16;
	*g_256 = g_16 * 16;
	*b_256 = b_16 * 16;
}


void _256_to_16_comp(int r_256, int g_256, int b_256, int *r_16, int *g_16, int *b_16)
{
	*r_16 = (int)((r_256 / 255.0) * 16.0);
	*g_16 = (int)((g_256 / 255.0) * 16.0);
	*b_16 = (int)((b_256 / 255.0) * 16.0);

	*r_16 = (*r_16 > 15 ? 15 : *r_16);
	*g_16 = (*g_16 > 15 ? 15 : *g_16);
	*b_16 = (*b_16 > 15 ? 15 : *b_16);
}


short linear_space(short x)
{
	float xf = x / 255.0f;
	float yf;

	if (xf <= 0.04045)
		yf = xf / 12.92;
	else
		yf = powf(((xf + 0.055) / 1.055), 2.4);
	return (int)(yf * 255);
}

float linear_space_f(float x)
{
	float xf = x / 255.0f;
	float yf;

	if (xf <= 0.04045)
		yf = xf / 12.92;
	else
		yf = powf(((xf + 0.055) / 1.055), 2.4);
	return yf * 255;
}

short rgb_space(short x)
{
	float xf = x / 255.0f;
	float yf;

	if (xf <= 0.0031308)
		yf = xf * 12.92;
	else
		yf = 1.055 * powf(xf, 1.0 / 2.4) - 0.055;
	return (int)(yf * 255);
}

float rgb_space_f(float x)
{
	float xf = x / 255.0f;
	float yf;

	if (xf <= 0.0031308)
		yf = xf * 12.92;
	else
		yf = 1.055 * (powf(xf, (1.0 / 2.4))) - 0.055;
	return yf * 255;
}






void guess_palette(IMAGE *image, PALETTE *palette)
{
	int block_x = image->width / 4;
	int block_y = image->height / 8;
	int color_index = 0;
	unsigned long sum[3], count, mid[3];
	int r, g, b;

	for (int py = 0; py < 8; py++) {
		for (int px = 0; px < 4; px++) {
			// Recherche de la couleur moyenne pour le bloc
			sum[0] = 0; sum[1] = 0; sum[2] = 0;
			count = 0;
			for (int y = 0; y < block_y; y++) {
				for (int x = 0; x < block_x; x++) {
					PIXEL p = image->pixels[(block_y * py * image->width) + y * image->width + (block_x * px) + x];
					sum[0] += p.r;
					sum[1] += p.g;
					sum[2] += p.b;
					count++;
				}
			}

			mid[0] = sum[0] / count;
			mid[1] = sum[1] / count;
			mid[2] = sum[2] / count;

			_256_to_16_comp(mid[0], mid[1], mid[2], &r, &g, &b);
			palette->colors[color_index][0] = r;
			palette->colors[color_index][1] = g;
			palette->colors[color_index][2] = b;

			color_index++;
		}
	}
	palette->size = 32;
	strcpy(palette->name, "guess");
}





// float color_delta_f(unsigned char c1[3], unsigned char c2[3])
// {
// 	int dr = abs((c1[0]) - (c2[0]));
// 	int dg = abs((c1[1]) - (c2[1]));
// 	int db = abs((c1[2]) - (c2[2]));

// 	return sqrt((dr * dr) + (dg * dg) + (db * db));
// }


float color_delta_f(unsigned char c1[3], unsigned char c2[3])
{
	int dr = abs((c1[0]) - (c2[0]));
	int dg = abs((c1[1]) - (c2[1]));
	int db = abs((c1[2]) - (c2[2]));

	return sqrt(30 * (dr * dr) + 59 * (dg * dg) + 11 * (db * db));
}


void convert_pixel(PIXEL_SHORT *source, PIXEL *target)
{
	target->r = max(min(source->r, 255), 0);
	target->g = max(min(source->g, 255), 0);
	target->b = max(min(source->b, 255), 0);
}





int find_closest_color_index(PIXEL *pixel, PALETTE *palette)
{
	// unsigned long d_plt;
	float d_plt;
	unsigned char c_current[3];
	// unsigned long diff = ULONG_MAX;
	float diff = FLT_MAX;
	unsigned char plt = 0;

	c_current[0] = pixel->r;
	c_current[1] = pixel->g;
	c_current[2] = pixel->b;

	// Recherche dans la palette
	for (int i = 0; i < palette->size; i++) {
		d_plt = color_delta_f(palette->colors[i], c_current);
		if (d_plt < diff) {
			plt = i;
			diff = d_plt;
		}
	}

	return plt;
}



void complementary_color(RGB_COLOR source, RGB_COLOR *target)
{
	int r = source[0];
	int g = source[1];
	int b = source[2];

	(*target)[0] = max(max(r, b), g) + min(min(r, b), g) - r;
	(*target)[1] = max(max(r, b), g) + min(min(r, b), g) - g;
	(*target)[2] = max(max(r, b), g) + min(min(r, b), g) - b;
}



void find_closest_color_rgb(PIXEL *pixel, PALETTE *palette, RGB_COLOR *color)
{
	// unsigned long d_plt;
	float d_plt;
	unsigned char thomson_c_current[3];
	// unsigned long diff = ULONG_MAX;
	float diff = FLT_MAX;
	unsigned char plt = 0;

	thomson_c_current[0] = pixel->r;
	thomson_c_current[1] = pixel->g;
	thomson_c_current[2] = pixel->b;

	// Recherche dans la palette
	for (int i = 0; i < palette->size; i++) {
		d_plt = color_delta_f(palette->colors[i], thomson_c_current);
		if (d_plt < diff) {
			plt = i;
			diff = d_plt;
		}
	}

	(*color)[0] = palette->colors[plt][0];
	(*color)[1] = palette->colors[plt][1];
	(*color)[2] = palette->colors[plt][2];
}




//------------------------------------------------------------------------------
// Retourne un pointeur sur une nouvelle image redimensionnee width*height
//------------------------------------------------------------------------------
IMAGE *bilinear_resize(IMAGE *image, unsigned short width, unsigned short height)
{
	unsigned short x_l, y_l, x_h, y_h;
	float x_weight, y_weight;

	PIXEL pixel, a, b, c, d;

	IMAGE *resized = (IMAGE *)malloc(sizeof(IMAGE));
	PIXEL *pixels = (PIXEL *)malloc(sizeof(PIXEL) * width * height);

	resized->pixels = pixels;
	resized->width = width;
	resized->height = height;


	float x_ratio = width > 1 ? ((float)(image->width - 1) / (width - 1)) : 0;
	float y_ratio = height > 1 ? ((float)(image->height - 1) / (height - 1)) : 0;

	// short percent = 0, old_percent = 0;

	for (unsigned short i = 0; i < height; i++) {
		/*
		 * percent = (int)((i / (float)height) * 100);
		 * if (percent != old_percent)
		 *      printf("Progression %d\n", percent);
		 * old_percent = percent;
		 */

		for (unsigned short j = 0; j < width; j++) {
			x_l = flr(x_ratio * j);
			y_l = flr(y_ratio * i);
			x_h = cl(x_ratio * j);
			y_h = cl(y_ratio * i);

			x_weight = (x_ratio * j) - x_l;
			y_weight = (y_ratio * i) - y_l;

			a = image->pixels[y_l * image->width + x_l];
			b = image->pixels[y_l * image->width + x_h];
			c = image->pixels[y_h * image->width + x_l];
			d = image->pixels[y_h * image->width + x_h];

			pixel.r = a.r * (1 - x_weight) * (1 - y_weight)
				  + b.r * x_weight * (1 - y_weight)
				  + c.r * y_weight * (1 - x_weight)
				  + d.r * x_weight * y_weight;

			pixel.g = a.g * (1 - x_weight) * (1 - y_weight)
				  + b.g * x_weight * (1 - y_weight)
				  + c.g * y_weight * (1 - x_weight)
				  + d.g * x_weight * y_weight;

			pixel.b = a.b * (1 - x_weight) * (1 - y_weight)
				  + b.b * x_weight * (1 - y_weight)
				  + c.b * y_weight * (1 - x_weight)
				  + d.b * x_weight * y_weight;

			resized->pixels[i * width + j] = pixel;
		}
	}

	return resized;
}



IMAGE *gamma(IMAGE *image, float gamma)
{
	PIXEL pixel, p;

	IMAGE *gamma_img = (IMAGE *)malloc(sizeof(IMAGE));
	PIXEL *pixels = (PIXEL *)malloc(sizeof(PIXEL) * image->width * image->height);

	float gamma_correction;

	gamma_img->pixels = pixels;
	gamma_img->width = image->width;
	gamma_img->height = image->height;

	for (unsigned short y = 0; y < image->height; y++) {
		for (unsigned short x = 0; x < image->width; x++) {
			p = image->pixels[y * image->width + x];

			gamma_correction = 1.0 / gamma;
			p.r = 255 * powf((p.r / 255.0), gamma_correction);
			p.g = 255 * powf((p.g / 255.0), gamma_correction);
			p.b = 255 * powf((p.b / 255.0), gamma_correction);

			gamma_img->pixels[y * image->width + x] = p;
		}
	}

	return gamma_img;
}


IMAGE *brightness_and_contrast(IMAGE *image, float brightness, float contrast)
{
	PIXEL pixel, p;

	IMAGE *br_ctr_img = (IMAGE *)malloc(sizeof(IMAGE));
	PIXEL *pixels = (PIXEL *)malloc(sizeof(PIXEL) * image->width * image->height);

	int r, g, b;

	br_ctr_img->pixels = pixels;
	br_ctr_img->width = image->width;
	br_ctr_img->height = image->height;

	for (unsigned short y = 0; y < image->height; y++) {
		for (unsigned short x = 0; x < image->width; x++) {
			p = image->pixels[y * image->width + x];

			r = (int)round(contrast * (p.r - 128.0) + 128.0 + brightness);
			g = (int)round(contrast * (p.g - 128.0) + 128.0 + brightness);
			b = (int)round(contrast * (p.b - 128.0) + 128.0 + brightness);

			r = r > 255 ? 255 : r;
			g = g > 255 ? 255 : g;
			b = b > 255 ? 255 : b;

			p.r = r < 0 ? 0 : r;
			p.g = g < 0 ? 0 : g;
			p.b = b < 0 ? 0 : b;
			br_ctr_img->pixels[y * image->width + x] = p;
		}
	}
	return br_ctr_img;
}



//------------------------------------------------------------------------------
// saturation l'image
//------------------------------------------------------------------------------
IMAGE *saturation(IMAGE *image, float sat)
{
	PIXEL pixel, p;

	IMAGE *sat_img = (IMAGE *)malloc(sizeof(IMAGE));
	PIXEL *pixels = (PIXEL *)malloc(sizeof(PIXEL) * image->width * image->height);

	float r, g, b;
	float gray;


	float Pr = 0.299f;
	float Pg = 0.587f;
	float Pb = 0.114f;

	sat_img->pixels = pixels;
	sat_img->width = image->width;
	sat_img->height = image->height;

	for (unsigned short y = 0; y < image->height; y++) {
		for (unsigned short x = 0; x < image->width; x++) {
			p = image->pixels[y * image->width + x];

			double P = sqrt(p.r * p.r * Pr + p.g * p.g * Pg + p.b * p.b * Pb);

			r = P + (p.r - P) * sat;
			g = P + (p.g - P) * sat;
			b = P + (p.b - P) * sat;

			r = r > 255 ? 255 : r;
			g = g > 255 ? 255 : g;
			b = b > 255 ? 255 : b;

			p.r = r < 0 ? 0 : r;
			p.g = g < 0 ? 0 : g;
			p.b = b < 0 ? 0 : b;

			sat_img->pixels[y * image->width + x] = p;
		}
	}
	return sat_img;
}




IMAGE *create_image(unsigned char *pixs, PALETTE *palette, int w, int h)
{
	IMAGE *image = (IMAGE *)malloc(sizeof(IMAGE));
	PIXEL *pixels = (PIXEL *)malloc(sizeof(PIXEL) * w * h);

	// int r, g, b;

	for (int y = 0; y < h; y++) {
		for (int x = 0; x < w; x++) {
			pixels[y * w + x].r = palette->colors[pixs[y * w + x]][0];
			pixels[y * w + x].g = palette->colors[pixs[y * w + x]][1];
			pixels[y * w + x].b = palette->colors[pixs[y * w + x]][2];
		}
	}

	image->width = w;
	image->height = h;
	image->pixels = pixels;

	return image;
}



IMAGE *create_empty_image(int w, int h)
{
	IMAGE *image = (IMAGE *)malloc(sizeof(IMAGE));

	if (image == NULL) {
		fprintf(stderr, "Impossible de créer la structure IMAGE\n");
		return NULL;
	}

	PIXEL *pixels = (PIXEL *)malloc(sizeof(PIXEL) * w * h);

	if (pixels == NULL) {
		fprintf(stderr, "Impossible de créer le tableau de PIXEL\n");
		return NULL;
	}


	// int r, g, b;

	for (int y = 0; y < h; y++) {
		for (int x = 0; x < w; x++) {
			pixels[y * w + x].r = 0;
			pixels[y * w + x].g = 0;
			pixels[y * w + x].b = 0;
		}
	}

	image->width = w;
	image->height = h;
	image->pixels = pixels;

	return image;
}



IMAGE *convert_rgb_image_to_linear(IMAGE *image)
{
	IMAGE *linear_img = (IMAGE *)malloc(sizeof(IMAGE));
	PIXEL *pixels = (PIXEL *)malloc(sizeof(PIXEL) * image->width * image->height);

	linear_img->pixels = pixels;
	linear_img->width = image->width;
	linear_img->height = image->height;

	PIXEL p;

	for (unsigned short y = 0; y < image->height; y++) {
		for (unsigned short x = 0; x < image->width; x++) {
			p = image->pixels[y * image->width + x];

			p.r = (int)round(linear_space_f(p.r));
			p.g = (int)round(linear_space_f(p.g));
			p.b = (int)round(linear_space_f(p.b));

			linear_img->pixels[y * image->width + x] = p;
		}
	}

	return linear_img;
}


IMAGE *convert_linear_image_to_rgb(IMAGE *image)
{
	IMAGE *rgb_img = (IMAGE *)malloc(sizeof(IMAGE));
	PIXEL *pixels = (PIXEL *)malloc(sizeof(PIXEL) * image->width * image->height);

	rgb_img->pixels = pixels;
	rgb_img->width = image->width;
	rgb_img->height = image->height;

	PIXEL p;

	for (unsigned short y = 0; y < image->height; y++) {
		for (unsigned short x = 0; x < image->width; x++) {
			p = image->pixels[y * image->width + x];

			p.r = (int)round(rgb_space_f(p.r));
			p.g = (int)round(rgb_space_f(p.g));
			p.b = (int)round(rgb_space_f(p.b));

			rgb_img->pixels[y * image->width + x] = p;
		}
	}

	return rgb_img;
}



unsigned char *create_pixels_array(IMAGE *image, PALETTE *palette)
{
	unsigned char *pixels = malloc(sizeof(unsigned char) * image->height * image->width);
	int index;

	if (!pixels) {
		printf("Pas assez de memoire\n");
		return NULL;
	}

	for (short y = 0; y < image->height; y++) {
		for (short x = 0; x < image->width; x++) {
			index = find_closest_color_index(&image->pixels[y * image->width + x], palette);
			pixels[y * image->width + x] = index;
		}
	}
	return pixels;
}






int get_color_value_from_index(int index, PALETTE *palette)
{
	int color = palette->colors[index][0] * 256 + palette->colors[index][1] * 16 + palette->colors[index][2];

	return color;
}



void load_palette(PALETTE *palette, const int colors[][3], int size)
{
	for (int i = 0; i < size; i++) {
		// printf("%d %d %d\n", colors[i][0], colors[i][1], colors[i][2]);
		palette->colors[i][0] = colors[i][0];
		palette->colors[i][1] = colors[i][1];
		palette->colors[i][2] = colors[i][2];
	}
	palette->size = size;
}


int load_hex_palette(char *filename, PALETTE *palette)
{
	char line[256];
	int count_lines = 0;
	int r, g, b;
	FILE *f = fopen(filename, "r");

	if (f == NULL)
		return 0;

	while (fgets(line, 256, f)) {
		sscanf(line, "%02X%02X%02X", &r, &g, &b);
		printf("%d %d %d\n", r, g, b);
		palette->colors[count_lines][0] = r;
		palette->colors[count_lines][1] = g;
		palette->colors[count_lines][2] = b;
		count_lines++;
	}
	palette->size = count_lines;


	return 1;
}


void generate_eq_space_palette(PALETTE *palette, int size)
{
	HSL hsl;
	RGBdec rgb;

	// dec2hsl(rgb, &hsl);

	srand(time(0));

	int count = 0;

	for (float i = 0; i < 360; i += (360 / 15)) {
		hsl.H = (int)i;
		hsl.S = 90.0 + (float)rand() / (float)(RAND_MAX / 10);
		hsl.L = 50.0 + (float)rand() / (float)(RAND_MAX / 10);
		showHSL(hsl);
		hsl2dec(hsl, &rgb);
		showRGBdec(rgb);
		palette->colors[count][0] = rgb.R;
		palette->colors[count][1] = rgb.G;
		palette->colors[count][2] = rgb.B;
		count++;
	}
	palette->colors[15][0] = 0;
	palette->colors[15][1] = 0;
	palette->colors[15][2] = 0;
}


void convert_linear_palette_to_rgb(PALETTE *palette)
{
	for (int i = 0; i < palette->size; i++) {
		palette->colors[i][0] = (int)round(rgb_space_f(palette->colors[i][0]));
		palette->colors[i][1] = (int)round(rgb_space_f(palette->colors[i][1]));
		palette->colors[i][2] = (int)round(rgb_space_f(palette->colors[i][2]));
	}
}


void convert_rgb_palette_to_linear(PALETTE *palette)
{
	for (int i = 0; i < palette->size; i++) {
		palette->colors[i][0] = (int)round(linear_space_f(palette->colors[i][0]));
		palette->colors[i][1] = (int)round(linear_space_f(palette->colors[i][1]));
		palette->colors[i][2] = (int)round(linear_space_f(palette->colors[i][2]));
	}
}
