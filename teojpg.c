#include "argparse.h"
#include "log.h"
#include "pixel.h"
#include "thomson.h"
#include "ordereddither.h"
#include "floyddither.h"
#include "mediancut.h"
#include "octree.h"
#include "wu.h"
#include "kmean.h"
#include "jpeg.h"

#include <SDL2/SDL.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdint.h>
#include <math.h>
#include <libgen.h>
#include <list.h>

#ifdef _WIN_
#include <windows.h>
#endif



#define MAX(x, y) (((x) > (y)) ? (x) : (y))
#define MIN(x, y) (((x) < (y)) ? (x) : (y))


static const char *const usages[] = {
	"teojpg [options] [[--] args]",
	"teojpg [options]",
	NULL,
};


SDL_Window *window;
SDL_Surface *screen;
SDL_Surface *surface;

unsigned char *pixels;
unsigned char *thomson_pixels;
IMAGE *the_image;
PALETTE the_palette;



int w = 640;
int h = 512;


//------------------------------------------------------------------------------
// Utils pour le log
//------------------------------------------------------------------------------
static FILE *f;
static void jpeg_log()
{
	f = fopen("teojpg.log", "w");
	log_add_fp(f, LOG_DEBUG);
	log_set_quiet(1);
}
static void end_jpeg_log()
{
	fclose(f);
}


char *fname(char *path)
{
	char *pfile;

	pfile = path + strlen(path);
	for (; pfile > path; pfile--) {
		if ((*pfile == '\\') || (*pfile == '/')) {
			pfile++;
			break;
		}
	}
	return pfile;
}

void replace_point(char *fname)
{
	while (*fname++)
		if (*fname == '.')
			*fname = '_';
}


int create_window(int true_color)
{
	SDL_Init(SDL_INIT_VIDEO);

	window = SDL_CreateWindow("",
				  SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
				  w, h,
				  0);

	screen = SDL_GetWindowSurface(window);

	if (!true_color)
		surface = SDL_CreateRGBSurface(SDL_SWSURFACE, w, h, 8, 0, 0, 0, 0);
	else
		surface = SDL_CreateRGBSurface(SDL_SWSURFACE, w, h, 32, 0, 0, 0, 0);


	return 1;
}



void draw_palette(IMAGE *image, PALETTE *palette)
{
	SDL_Color colors[255];

	for (int i = 0; i < palette->size; i++) {
		colors[i].r = palette->colors[i][0] * 16;
		colors[i].g = palette->colors[i][1] * 16;
		colors[i].b = palette->colors[i][2] * 16;
		colors[i].a = 255;
	}

	SDL_SetPaletteColors(surface->format->palette, colors, 0, palette->size);

	int block_x = w / 4;
	int block_y = h / 8;
	int color_index = 0;

	puts(SDL_GetPixelFormatName(surface->format->format));

	unsigned char *pixels = surface->pixels;

	for (int py = 0; py < 8; py++) {
		for (int px = 0; px < 4; px++) {
			for (int y = 0; y < block_y; y++)
				for (int x = 0; x < block_x; x++)
					pixels[(px * block_x + x) + (py * block_y + y) * w] = color_index;
			color_index++;
		}
	}
}


void draw_box(int xx, int yy, int width, int height, unsigned char color)
{
	unsigned char *sdl_pixels = surface->pixels;

	for (int y = 0; y < height; y++)
		for (int x = 0; x < width; x++)
			sdl_pixels[xx + x + ((yy + y) * w)] = color;
}


void draw_pixels(unsigned char *pixels, int width, int height, PALETTE *palette)
{
	SDL_Color colors[255];
	int r, g, b;

	for (int i = 0; i < palette->size; i++) {
		colors[i].r = palette->colors[i][0];
		colors[i].g = palette->colors[i][1];
		colors[i].b = palette->colors[i][2];
		colors[i].a = 255;
	}

	SDL_SetPaletteColors(surface->format->palette, colors, 0, palette->size);

	unsigned char *sdl_pixels = surface->pixels;

	for (int y = 0; y < h; y++)
		for (int x = 0; x < w; x++)
			sdl_pixels[x + y * w] = 0;

	for (int y = 0; y < height; y++) {
		for (int x = 0; x < width; x++) {
			sdl_pixels[(x * 2) + (y * 2 * w)] = pixels[x + y * width];
			sdl_pixels[(x * 2 + 1) + (y * 2 * w)] = pixels[x + y * width];
			sdl_pixels[(x * 2) + ((y * 2 + 1) * w)] = pixels[x + y * width];
			sdl_pixels[(x * 2 + 1) + ((y * 2 + 1) * w)] = pixels[x + y * width];
		}
	}

	int step = w / palette->size;

	for (int i = 0; i < palette->size; i++)
		draw_box(i * step, 500, step, 12, i);
}


void draw_pixels_x_1(unsigned char *pixels, int width, int height, PALETTE *palette)
{
	SDL_Color colors[255];
	int r, g, b;

	for (int i = 0; i < palette->size; i++) {
		colors[i].r = palette->colors[i][0];
		colors[i].g = palette->colors[i][1];
		colors[i].b = palette->colors[i][2];
		colors[i].a = 255;
	}

	SDL_SetPaletteColors(surface->format->palette, colors, 0, palette->size);

	unsigned char *sdl_pixels = surface->pixels;

	for (int y = 0; y < h; y++)
		for (int x = 0; x < w; x++)
			sdl_pixels[x + y * w] = 0;

	for (int y = 0; y < height; y++)
		for (int x = 0; x < width; x++)
			sdl_pixels[x + (y * w)] = pixels[x + y * width];

	int step = w / palette->size;

	for (int i = 0; i < palette->size; i++)
		draw_box(i * step, 500, step, 12, i);
}



void draw_pixels_BM16(unsigned char *pixels, int width, int height, PALETTE *palette)
{
	SDL_Color colors[255];
	int r, g, b;

	for (int i = 0; i < palette->size; i++) {
		colors[i].r = palette->colors[i][0];
		colors[i].g = palette->colors[i][1];
		colors[i].b = palette->colors[i][2];
		colors[i].a = 255;
	}

	SDL_SetPaletteColors(surface->format->palette, colors, 0, palette->size);

	unsigned char *sdl_pixels = surface->pixels;

	for (int y = 0; y < h; y++)
		for (int x = 0; x < w; x++)
			sdl_pixels[x + y * w] = 0;

	for (int y = 0; y < height; y++) {
		for (int x = 0; x < width; x++) {
			sdl_pixels[(x * 4) + (y * 2 * w)] = pixels[x + y * width];
			sdl_pixels[(x * 4 + 1) + (y * 2 * w)] = pixels[x + y * width];
			sdl_pixels[(x * 4 + 2) + (y * 2 * w)] = pixels[x + y * width];
			sdl_pixels[(x * 4 + 3) + (y * 2 * w)] = pixels[x + y * width];
			sdl_pixels[(x * 4) + ((y * 2 + 1) * w)] = pixels[x + y * width];
			sdl_pixels[(x * 4 + 1) + ((y * 2 + 1) * w)] = pixels[x + y * width];
			sdl_pixels[(x * 4 + 2) + ((y * 2 + 1) * w)] = pixels[x + y * width];
			sdl_pixels[(x * 4 + 3) + ((y * 2 + 1) * w)] = pixels[x + y * width];
		}
	}

	int step = w / palette->size;

	for (int i = 0; i < palette->size; i++)
		draw_box(i * step, 500, step, 12, i);
}


void draw_pixels_BM16_x_1(unsigned char *pixels, int width, int height, PALETTE *palette)
{
	SDL_Color colors[255];
	int r, g, b;

	for (int i = 0; i < palette->size; i++) {
		colors[i].r = palette->colors[i][0];
		colors[i].g = palette->colors[i][1];
		colors[i].b = palette->colors[i][2];
		colors[i].a = 255;
	}

	SDL_SetPaletteColors(surface->format->palette, colors, 0, palette->size);

	unsigned char *sdl_pixels = surface->pixels;

	for (int y = 0; y < h; y++)
		for (int x = 0; x < w; x++)
			sdl_pixels[x + y * w] = 0;

	for (int y = 0; y < height; y++) {
		for (int x = 0; x < width; x++) {
			sdl_pixels[(x * 2) + (y * w)] = pixels[x + y * width];
			sdl_pixels[(x * 2 + 1) + (y * w)] = pixels[x + y * width];
		}
	}

	int step = w / palette->size;

	for (int i = 0; i < palette->size; i++)
		draw_box(i * step, 500, step, 12, i);
}



void redraw_BM40(int *pixels_plus, int *clash_mode)
{
	if (*pixels_plus) {
		if (*clash_mode == 1)
			draw_pixels(thomson_pixels, the_image->width, the_image->height, &the_palette);
		else
			draw_pixels(pixels, the_image->width, the_image->height, &the_palette);
	} else {
		if (*clash_mode == 1)
			draw_pixels_x_1(thomson_pixels, the_image->width, the_image->height, &the_palette);
		else
			draw_pixels_x_1(pixels, the_image->width, the_image->height, &the_palette);
	}
}


void redraw_BM16(int *pixels_plus)
{
	if (*pixels_plus == 1)
		draw_pixels_BM16(pixels, the_image->width, the_image->height, &the_palette);
	else
		draw_pixels_BM16_x_1(pixels, the_image->width, the_image->height, &the_palette);
}


int message_loop(char *bm_mode)
{
	int clash_mode = 0;
	int exit = 0;
	int pixels_plus = 1;

	while (1) {
		if (exit) break;

		SDL_Event event;
		while (SDL_PollEvent(&event)) {
			if (event.type == SDL_QUIT) exit = 1;

			if (strcmp(bm_mode, "BM40") == 0) {
				switch (event.type) {
				case SDL_KEYUP:
					if (event.key.keysym.sym == SDLK_SPACE) {
						if (clash_mode == 1)
							clash_mode = 0;
						else
							clash_mode = 1;
						redraw_BM40(&pixels_plus, &clash_mode);
					}
					if (event.key.keysym.sym == SDLK_ESCAPE)
						exit = 1;

					if (event.key.keysym.sym == SDLK_KP_PLUS) {
						pixels_plus = 1;
						redraw_BM40(&pixels_plus, &clash_mode);
					}

					if (event.key.keysym.sym == SDLK_KP_MINUS) {
						pixels_plus = 0;
						redraw_BM40(&pixels_plus, &clash_mode);
					}

					break;
				default:
					break;
				}
			} else if (strcmp(bm_mode, "BM16") == 0) {
				switch (event.type) {
				case SDL_KEYUP:
					if (event.key.keysym.sym == SDLK_ESCAPE)
						exit = 1;

					if (event.key.keysym.sym == SDLK_KP_PLUS) {
						pixels_plus = 1;
						redraw_BM16(&pixels_plus);
					}

					if (event.key.keysym.sym == SDLK_KP_MINUS) {
						pixels_plus = 0;
						redraw_BM16(&pixels_plus);
					}

					break;
				default:
					break;
				}
			} else if (strcmp(bm_mode, "BM4") == 0) {
				switch (event.type) {
				case SDL_KEYUP:
					if (event.key.keysym.sym == SDLK_ESCAPE)
						exit = 1;

					if (event.key.keysym.sym == SDLK_KP_PLUS) {
						pixels_plus = 1;
						redraw_BM40(&pixels_plus, &clash_mode);
					}

					if (event.key.keysym.sym == SDLK_KP_MINUS) {
						pixels_plus = 0;
						redraw_BM40(&pixels_plus, &clash_mode);
					}

					break;
				default:
					break;
				}
			}
		}

		//
		// Display the pixel buffer here.
		//
		SDL_BlitSurface(surface, NULL, screen, NULL);
		SDL_UpdateWindowSurface(window);
		SDL_Delay(100);
	}
}



int main(int argc, char *argv[])
{
	char image_filename[256];
	char palette_name[256];

	float *the_matrix;
	int the_matrix_size[2];
	int matrix_size = 0;
	float gammaf = 1.0f;
	float brightnessf = 0.0f;
	float contrastf = 1.0f;
	float saturationf = 0.0f;

	MACHINE_CONF *the_conf;

	jpeg_log();


	// Lecture des arguments
	const char *arg_input = NULL;
	const char *arg_palette = NULL;
	const char *arg_machine = NULL;
	int arg_matrix = 0;
	int arg_floyd = 0;
	enum DITHER { MATRIX, FLOYD, OSTRO };

	int dither_mode;
	float arg_gamma = 0.f;
	float arg_contrast = 0.f;
	float arg_brightness = 0.f;
	float arg_saturation = 0.f;
	int arg_octree = 0;
	int arg_median_cut = 0;
	int arg_wu = 0;
	int arg_ostro = 0;
	int arg_kmean = 0;
	const char *arg_mode = NULL;
	char the_mode[10];

	struct argparse_option arg_options[] = {
		OPT_HELP(),
		OPT_GROUP("Basic options"),
		OPT_STRING('i',		   "input",	    &arg_input,	     "Fichier image jpg",					 NULL, 0, 0),
		OPT_STRING('p',		   "palette",	    &arg_palette,    "Fichier palette format hex",				 NULL, 0, 0),
		OPT_INTEGER('m',	   "matrix",	    &arg_matrix,     "Tramage ordonné Matrice [2,3,4,7,8]",			 NULL, 0, 0),
		OPT_INTEGER('f',	   "floyd",	    &arg_floyd,	     "Tramage Floyd Steinberg [1->10]",				 NULL, 0, 0),
		OPT_BOOLEAN('k',	   "ostromoukhov",  &arg_ostro,	     "Tramage adaptif ostromoukof",				 NULL, 0, 0),
		OPT_FLOAT('g',		   "gamma",	    &arg_gamma,	     "Correction gamma [0.1->2.0]",				 NULL, 0, 0),
		OPT_FLOAT('b',		   "brightness",    &arg_brightness, "Luminosité [-128->+128]",					 NULL, 0, 0),
		OPT_FLOAT('t',		   "contrast",	    &arg_contrast,   "Contraste [0.1->2.0]",					 NULL, 0, 0),
		OPT_FLOAT('s',		   "saturation",    &arg_saturation, "Saturation [0.1->2.0]",					 NULL, 0, 0),
		OPT_BOOLEAN('e',	   "kmean",	    &arg_kmean,	     "Quantification K-mean",					 NULL, 0, 0),
		OPT_BOOLEAN('w',	   "wu",	    &arg_wu,	     "Quantification Wu",					 NULL, 0, 0),
		OPT_BOOLEAN('o',	   "octree",	    &arg_octree,     "Quantification Octree",					 NULL, 0, 0),
		OPT_BOOLEAN('c',	   "median",	    &arg_median_cut, "Quantification median-cut",				 NULL, 0, 0),
		OPT_STRING('z',		   "machine",	    &arg_machine,    "Génération pour thomson [TO7, TO770, MO5, MO6, TO8, TO9]", NULL, 0, 0),
		OPT_STRING('d',		   "mode",	    &arg_mode,	     "Mode graphique [BM40, BM16, BM4, BM80]",			 NULL, 0, 0),
		OPT_END(),
	};

	struct argparse argparse;

	argparse_init(&argparse, arg_options, usages, 0);
	argparse_describe(&argparse, "\nA brief description of what the program does and how it works.", "\nAdditional description of the program after the description of the arguments.");
	argc = argparse_parse(&argparse, argc, (const char **)argv);


	if (arg_machine && strcmp(arg_machine, "TO7") == 0) {
		the_conf = &TO7_CONF;
		printf("REM Configuration TO7\n");
	} else if (arg_machine && strcmp(arg_machine, "TO770") == 0) {
		the_conf = &TO7_70_CONF;
		printf("Configuration TO7 70\n");
	} else if (arg_machine && strcmp(arg_machine, "MO5") == 0) {
		the_conf = &MO5_CONF;
		printf("Configuration MO5\n");
	} else if (arg_machine && strcmp(arg_machine, "MO6") == 0) {
		the_conf = &MO6_CONF;
		printf("Configuration MO6\n");
	} else if (arg_machine && strcmp(arg_machine, "TO8") == 0) {
		the_conf = &TO8_CONF;
		printf("Configuration TO8\n");
	} else if (arg_machine && strcmp(arg_machine, "TO9") == 0) {
		the_conf = &TO9_CONF;
		printf("Configuration TO9\n");
	} else {
		// defaut -> TO9
		the_conf = &TO9_CONF;
		printf("Configuration non reconnue -> TO9 par defaut\n");
	}




	if (arg_mode) {
		// Il faut vérifier que le mode est compatible avec la machine

		int *modes = the_conf->modes;
		int can_do_mode = 0;
		for (int i = 0; i < 5; i++)
			if (strcmp(BM_MODE_STRING[the_conf->modes[i]], arg_mode) == 0)
				can_do_mode = 1;
		if (!can_do_mode) {
			printf("La machine sélectionnée ne peut produire ce mode graphique");
			return 0;
		}
		strcpy(the_mode, arg_mode);
		printf("Mode graphique sélectionné %s\n", the_mode);
	} else {
		printf("Mode graphique par défaut BM40\n");
		strcpy(the_mode, "BM40");       // mode par défaut
	}


	if (arg_matrix) {
		matrix_size = arg_matrix;
		printf("Dithering ordonne %d\n", matrix_size);
		dither_mode = MATRIX;
	} else if (arg_floyd > 0 && arg_floyd <= 10) {
		printf("Dithering Floyd Steinberg - utilisation de la matrice %s\n", floyd_matrix[arg_floyd - 1].name);
		matrix_size = 0;
		dither_mode = FLOYD;
	} else if (arg_floyd > 0 && arg_floyd > 10) {
		printf("Dithering Floyd Steinberg - utilisation de la matrice standard\n", arg_floyd);
		matrix_size = 0;
	} else if (arg_ostro) {
		dither_mode = OSTRO;
	}


	if (arg_input != NULL) {
		strcpy(image_filename, arg_input);
		printf("REM Chargement de %s\n", image_filename);
	} else {
		printf("Le nom du fichier JPEG doit être passé en paramètres\n");
		exit(0);
	}


	the_image = load(image_filename);
	if (!the_image)
		return 0;


	float ratio_x = 0, ratio_y = 0, ratio;
	int do_resize = 0;


	if (strcmp(the_mode, "BM16") != 0 && the_image->width >= 320) {
		ratio_x = the_image->width / 320.0;
		printf("ratio x %f\n", ratio_x);
		do_resize = 1;
	} else if (strcmp(the_mode, "BM16") == 0 && the_image->width >= 160) {
		ratio_x = the_image->width / 320.0;
		printf("ratio x %f\n", ratio_x);
		do_resize = 1;
	}
	if (the_image->height > 200) {
		ratio_y = the_image->height / 200.0;
		printf("ratio y %f\n", ratio_y);
		do_resize = 1;
	}

	if (do_resize) {
		ratio = MAX(ratio_x, ratio_y);
		printf("Redimensionnement ratio %f\n", ratio);

		IMAGE *new_image = NULL;
		if (strcmp(the_mode, "BM16") != 0) {
			new_image = bilinear_resize(the_image,
						    (int)the_image->width / ratio,
						    (int)the_image->height / ratio);
		} else {
			new_image = bilinear_resize(the_image,
						    (int)the_image->width / (ratio * 2), // pixels doubles
						    (int)the_image->height / ratio);
		}


		free_image(the_image);
		the_image = new_image;
		printf("Nouvelle dimensions %d %d\n", the_image->width, the_image->height);
	}


	int thomson_width = (the_image->width / 8) * 8;

	printf("Mode Thomson Palette 16 couleurs et surface traitees %d*%d\n", thomson_width, the_image->height);

	// palette
	int iter_median_cut = 4;

	if (arg_gamma != 0) {
		gammaf = arg_gamma;
		printf("Correction gamma %f\n", gammaf);
		IMAGE *gamma_image = gamma(the_image, gammaf);
		free_image(the_image);
		the_image = gamma_image;
	}


	if (arg_contrast != 0) {
		contrastf = arg_contrast;
		printf("Correction contraste %f\n", contrastf);
		IMAGE *contrast_image = brightness_and_contrast(the_image, 0.0f, contrastf);
		free_image(the_image);
		the_image = contrast_image;
	}

	if (arg_brightness != 0) {
		brightnessf = arg_brightness;
		printf("Correction luminosite %f\n", brightnessf);
		IMAGE *brighntess_image = brightness_and_contrast(the_image, brightnessf, 1.0f);
		free_image(the_image);
		the_image = brighntess_image;
	}


	if (arg_saturation != 0) {
		saturationf = arg_saturation;
		printf("Correction saturation %f\n", saturationf);
		IMAGE *saturation_image = saturation(the_image, saturationf);
		free_image(the_image);
		the_image = saturation_image;
	}


	init_thomson_linear_levels();


	if (the_conf->has_4096_colors) {
		if (arg_palette) {
			// Dans ce cas fichier palette prédefinie
			int l = load_hex_palette((char *)arg_palette, &the_palette);
			if (!l) {
				printf("Impossible de charger la palette\n");
				return 0;
			}

			printf("Utilisation de la palette prédéfinie %s\n", arg_palette);
			printf("Nombre de couleurs:%d\n", the_palette.size);

			if (strcmp(the_mode, "BM4") == 0 && the_palette.size > 4) {
				// Le nombre de couleur est trop élevé
				printf("Mode BM4 - 4 premières couleurs de la palette effectives\n");
				the_palette.size = 4;
			} else if (the_palette.size > 16) {
				printf("Nombre de couleurs de la palette > 16 - Prise en compte des 16 premières couleurs\n");
				the_palette.size = 16;
			}
		} else {
			// Sinon utilisation d'un algo de quantification

			int color_max = 16;
			if (strcmp(the_mode, "BM4") == 0) {
				printf("Reduction à 4 couleurs\n");
				color_max = 4;
				iter_median_cut = 2;
			}

			// Dans ce cas, il faut chercher une palette
			if (arg_median_cut) {
				printf("Reduction de palette median-cut\n");
				guess_palette_median_cut(the_image, &the_palette, iter_median_cut);
			} else if (arg_octree) {
				printf("Reduction de palette octree\n");
				guess_palette_octree(the_image, &the_palette, /*16*/ color_max);
			} else if (arg_wu) {
				printf("Reduction de palette wu\n");
				guess_palette_wu(the_image, &the_palette, /*16*/ color_max);
			} else /*if (arg_kmean)*/ {
				printf("Reduction de palette k-mean\n");
				guess_palette_kmean(the_image, &the_palette, color_max);
			}

			// post traitemnt sur les couleurs trouvées
			// augmentation de la luminosité et noir à
			// la place de la couleur la plus sombre
			if (color_max == 16) {
				thomson_post_trt_palette_16(&the_palette, &the_palette);
			} else {
				thomson_post_trt_palette_4(&the_palette, &the_palette);
			}
		}
	} else {
		// Palette fixe de l'ordinateur
		printf("Palette fixe %d couleurs\n", the_conf->colors_count);
		load_palette(&the_palette, the_conf->palette, the_conf->colors_count);
	}

	fflush(stdout);


	// passage en CIE XYZ pour le dithering
	IMAGE *cie_image = convert_rgb_image_to_linear(the_image);

	free_image(the_image);
	the_image = cie_image;
	convert_rgb_palette_to_linear(&the_palette);





	if (/*!floyd*/ dither_mode == MATRIX) {
		// dithering ordonné
		if (matrix_size == 0) {
			the_matrix = NULL;
		} else if (matrix_size == 2) {
			the_matrix = malloc(BAYER_2_2_SIZE[0] * BAYER_2_2_SIZE[1] * sizeof(float));
			the_matrix_size[0] = BAYER_2_2_SIZE[0];
			the_matrix_size[1] = BAYER_2_2_SIZE[1];
			get_precalculated_matrix(BAYER_2_2, BAYER_2_2_SIZE, the_matrix);
		} else if (matrix_size == 3) {
			the_matrix = malloc(BAYER_3_3_SIZE[0] * BAYER_3_3_SIZE[1] * sizeof(float));
			the_matrix_size[0] = BAYER_3_3_SIZE[0];
			the_matrix_size[1] = BAYER_3_3_SIZE[1];
			get_precalculated_matrix(BAYER_3_3, BAYER_3_3_SIZE, the_matrix);
		} else if (matrix_size == 4) {
			the_matrix = malloc(BAYER_4_4_SIZE[0] * BAYER_4_4_SIZE[1] * sizeof(float));
			the_matrix_size[0] = BAYER_4_4_SIZE[0];
			the_matrix_size[1] = BAYER_4_4_SIZE[1];
			get_precalculated_matrix(BAYER_4_4, BAYER_4_4_SIZE, the_matrix);
		} else if (matrix_size == 8) {
			the_matrix = malloc(BAYER_8_8_SIZE[0] * BAYER_8_8_SIZE[1] * sizeof(float));
			the_matrix_size[0] = BAYER_8_8_SIZE[0];
			the_matrix_size[1] = BAYER_8_8_SIZE[1];
			get_precalculated_matrix(BAYER_8_8, BAYER_8_8_SIZE, the_matrix);
		} else if (matrix_size == 7) {
			the_matrix = malloc(BAYER_15_7[0] * BAYER_15_7[1] * sizeof(float));
			the_matrix_size[0] = BAYER_15_7_SIZE[0];
			the_matrix_size[1] = BAYER_15_7_SIZE[1];
			get_precalculated_matrix(BAYER_15_7, BAYER_15_7_SIZE, the_matrix);
		}

		IMAGE *ordered_image = ordered_dither_yliluoma(the_image, &the_palette, the_matrix, the_matrix_size);
		// IMAGE *ordered_image = ordered_dither(the_image, &the_palette, the_matrix, the_matrix_size);
		free_image(the_image);
		the_image = ordered_image;
		free(the_matrix);
	} else if (dither_mode == FLOYD) {
		// dithering type Floyd Steinberg
		IMAGE *fs_image = floyd_steinberg_dither(the_image, &the_palette, &floyd_matrix[arg_floyd - 1]);
		free_image(the_image);
		the_image = fs_image;
	} else {
		// Ostromoukhov
		IMAGE *fs_image = ostromoukhov_dither(the_image, &the_palette);
		free_image(the_image);
		the_image = fs_image;
	}




	// passage en RGB pour l'affichage
	IMAGE *rgb_image = convert_linear_image_to_rgb(the_image);

	free_image(the_image);
	the_image = rgb_image;
	convert_linear_palette_to_rgb(&the_palette);

	pixels = create_pixels_array(the_image, &the_palette);

	if (strcmp(the_mode, "BM4") == 0) {
		char binary_string[9] = { 0 };
		char byte[9] = { 0 };
		int k;
		// Sauvegarde TO-SNAP
		MAP_SEG map_4;
		init_map_seg(&map_4);

		// Garde 8 car max du nom du fichier
		char *filename_without_path;
		char filename_only[256];

		filename_without_path = fname(image_filename);
		if (strlen(filename_without_path) > 8)
			filename_without_path[8] = 0;
		replace_point(filename_without_path);

		// Sauvegarde basic
		save_bm4_basic(the_image, pixels, &the_palette, filename_without_path);

		map_4.lines = the_image->height / 8;
		map_4.columns = the_image->width / 8;

		int x_count = 0;
		int bit_idx = 0;
		unsigned char the_byte_a = 0, the_byte_b = 0;
		for (int y = 0; y < map_4.lines * 8; y++) {
			for (int x = 0; x < map_4.columns * 8; x += 1) {
				// for (int y = 0; y < map_4.lines * 8; y++) {
				k = pixels[y * the_image->width + x];
				if (k == 0) {
					the_byte_a = the_byte_a | (0 << (7 - bit_idx));
					the_byte_b = the_byte_b | (0 << (7 - bit_idx));
				} else if (k == 1) {
					the_byte_a = the_byte_a | (0 << (7 - bit_idx));
					the_byte_b = the_byte_b | (1 << (7 - bit_idx));
				} else if (k == 2) {
					the_byte_a = the_byte_a | (1 << (7 - bit_idx));
					the_byte_b = the_byte_b | (0 << (7 - bit_idx));
				} else {
					// k = 3
					the_byte_a = the_byte_a | (1 << (7 - bit_idx));
					the_byte_b = the_byte_b | (1 << (7 - bit_idx));
				}

				bit_idx++;
				if (bit_idx == 8) {
					bit_idx = 0;
					list_add_last(map_4.rama, &the_byte_a);
					list_add_last(map_4.ramb, &the_byte_b);
					the_byte_a = 0;
					the_byte_b = 0;
				}
			}
		}
		int a = list_size(map_4.rama);
		int b = list_size(map_4.ramb);

		save_map_4(filename_without_path, &map_4, &the_palette);


		// Affichage
		if (!create_window(0)) {
			printf("Impossible de creer l'ecran\n");
			return 0;
		}


		free_map_seg(&map_4);

		printf("Affichage du résultat SDL\n");
		draw_pixels(pixels, the_image->width, the_image->height, &the_palette);

		printf("+/- : zoom\n");
		fflush(stdout);

		SDL_SetWindowTitle(window, "+/- : zoom");
	} else if (strcmp(the_mode, "BM40") == 0) {
		// Gestion et affichage du mode BM40

		MAP_SEG map_40;
		init_map_seg(&map_40);

		// Garde 8 car max du nom du fichier
		char *filename_without_path;
		char filename_only[256];

		filename_without_path = fname(image_filename);
		if (strlen(filename_without_path) > 8)
			filename_without_path[8] = 0;
		replace_point(filename_without_path);

		// Recherche color clash
		thomson_pixels = thomson_post_trt_combin(the_image, &the_palette, &map_40, the_conf, filename_without_path);

		// Sauvegarde TO-SNAP
		save_map_40_col(filename_without_path, &map_40, &the_palette);
		free_map_seg(&map_40);

		// Affichage
		if (!create_window(0)) {
			printf("Impossible de creer l'ecran\n");
			return 0;
		}

		printf("Affichage du résultat SDL\n");
		draw_pixels(pixels, the_image->width, the_image->height, &the_palette);

		printf("Espace : toggle color clash ou pas\n");
		printf("+/- : zoom\n");
		fflush(stdout);

		SDL_SetWindowTitle(window, "Espace : toggle color clash | +/- : zoom");
	} else if (strcmp(the_mode, "BM16") == 0) {
		// Gestion et affichage du mode BM16 (pixels doubles)

		MAP_SEG map_16;
		init_map_seg(&map_16);

		// Garde 8 car max du nom du fichier
		char *filename_without_path;
		char filename_only[256];
		unsigned char two_pixels;

		filename_without_path = fname(image_filename);
		if (strlen(filename_without_path) > 8)
			filename_without_path[8] = 0;
		replace_point(filename_without_path);

		// Sauvegarde basic
		save_bm16_basic(the_image, pixels, &the_palette, filename_without_path);


		// Sauvegarde TO-SNAP BM16
		map_16.lines = the_image->height / 8;
		map_16.columns = the_image->width / 2;
		int x_count = 0;
		for (int x = 0; x < map_16.columns * 2; x += 4) {
			for (int y = 0; y < map_16.lines * 8; y++) {
				// RAMA
				two_pixels = pixels[y * the_image->width + x] << 4 | pixels[y * the_image->width + x + 1];
				list_add_last(map_16.rama, &two_pixels);

				// RAMB
				two_pixels = pixels[y * the_image->width + x + 2] << 4 | pixels[y * the_image->width + x + 3];
				list_add_last(map_16.ramb, &two_pixels);
			}
			x_count++;
		}

		save_map_16(filename_without_path, &map_16, &the_palette, x_count);
		free_map_seg(&map_16);

		// Affichage
		if (!create_window(0)) {
			printf("Impossible de creer l'ecran\n");
			return 0;
		}
		printf("Affichage du résultat SDL\n");
		draw_pixels_BM16(pixels, the_image->width, the_image->height, &the_palette);
		fflush(stdout);
		SDL_SetWindowTitle(window, "+/- : zoom");
	}

	message_loop(the_mode);

	free(pixels);
	free(thomson_pixels);
	free_image(the_image);

	end_jpeg_log();
}


#ifdef _WIN_
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevIns, LPSTR lpszArgument, int iShow)
{
	LPWSTR *szArglist;

	int nArgs;
	int i;

	szArglist = CommandLineToArgvW(GetCommandLineW(), &nArgs);

	char *argv[nArgs];

	if (NULL == szArglist) {
		wprintf(L"CommandLineToArgvW failed\n");
		return 0;
	} else {
		for (i = 0; i < nArgs; i++) {
			argv[i] = malloc(255);
			wcstombs(argv[i], szArglist[i], 255);
		}
	}

	// Free memory allocated for CommandLineToArgvW arguments.
	LocalFree(szArglist);

	main(nArgs, argv);
	for (i = 0; i < nArgs; i++)
		free(argv[i]);

	return 0;
}
#endif
