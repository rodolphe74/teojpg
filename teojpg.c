#include "argparse.h"
#include "log.h"
#include "pixel.h"
#include "thomson.h"
#include "ordereddither.h"
#include "floyddither.h"
#include "mediancut.h"
#include "octree.h"
#include "wu.h"
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



void redraw(int *pixels_plus, int *next_pixels)
{
	if (*pixels_plus) {
		if (*next_pixels == 1)
			draw_pixels(thomson_pixels, the_image->width, the_image->height, &the_palette);
		else
			draw_pixels(pixels, the_image->width, the_image->height, &the_palette);
	} else {
		if (*next_pixels == 1)
			draw_pixels_x_1(thomson_pixels, the_image->width, the_image->height, &the_palette);
		else
			draw_pixels_x_1(pixels, the_image->width, the_image->height, &the_palette);
	}
}


int message_loop()
{
	int next_pixels = 0;
	int exit = 0;
	int pixels_plus = 1;

	while (1) {
		if (exit) break;

		SDL_Event event;
		while (SDL_PollEvent(&event)) {
			if (event.type == SDL_QUIT) exit = 1;

			switch (event.type) {
			case SDL_KEYUP:
				if (event.key.keysym.sym == SDLK_SPACE) {
					if (next_pixels == 1)
						next_pixels = 0;
					else
						next_pixels = 1;
					redraw(&pixels_plus, &next_pixels);
				}
				if (event.key.keysym.sym == SDLK_ESCAPE)
					exit = 1;

				if (event.key.keysym.sym == SDLK_KP_PLUS) {
					pixels_plus = 1;
					redraw(&pixels_plus, &next_pixels);
				}

				if (event.key.keysym.sym == SDLK_KP_MINUS) {
					pixels_plus = 0;
					redraw(&pixels_plus, &next_pixels);
				}

				break;
			default:
				break;
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
	int floyd = 0;
	float gammaf = 1.0f;
	float brightnessf = 0.0f;
	float contrastf = 1.0f;
	float saturationf = 0.0f;

	MACHINE_CONF *the_conf;

	jpeg_log();


	// Lecture des arguments
	const char *arg_input = NULL;
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

	struct argparse_option arg_options[] = {
		OPT_HELP(),
		OPT_GROUP("Basic options"),
		OPT_STRING('i',		   "input",	    &arg_input,	     "Nom de l'image",						 NULL, 0, 0),
		OPT_INTEGER('m',	   "matrix",	    &arg_matrix,     "Tramage ordonné Matrice [2,3,4,8]",			 NULL, 0, 0),
		OPT_INTEGER('f',	   "floyd",	    &arg_floyd,	     "Tramage Floyd Steinberg [1->10]",				 NULL, 0, 0),
		OPT_BOOLEAN('k',	   "ostromoukhov",  &arg_ostro,	     "Tramage adaptif ostromoukof",				 NULL, 0, 0),
		OPT_FLOAT('g',		   "gamma",	    &arg_gamma,	     "Correction gamma [0.1->2.0]",				 NULL, 0, 0),
		OPT_FLOAT('b',		   "brightness",    &arg_brightness, "Luminosité [-128->+128]",					 NULL, 0, 0),
		OPT_FLOAT('t',		   "contrast",	    &arg_contrast,   "Contraste [0.1->2.0]",					 NULL, 0, 0),
		OPT_FLOAT('s',		   "saturation",    &arg_saturation, "Saturation [0.1->2.0]",					 NULL, 0, 0),
		OPT_BOOLEAN('w',	   "wu",	    &arg_wu,	     "Quantification Wu",					 NULL, 0, 0),
		OPT_BOOLEAN('o',	   "octree",	    &arg_octree,     "Quantification Octree",					 NULL, 0, 0),
		OPT_BOOLEAN('c',	   "median",	    &arg_median_cut, "Quantification median-cut",				 NULL, 0, 0),
		OPT_STRING('z',		   "machine",	    &arg_machine,    "Génération pour thomson [TO7, TO770, MO5, MO6, TO8, TO9]", NULL, 0, 0),
		OPT_END(),
	};

	struct argparse argparse;

	argparse_init(&argparse, arg_options, usages, 0);
	argparse_describe(&argparse, "\nA brief description of what the program does and how it works.", "\nAdditional description of the program after the description of the arguments.");
	argc = argparse_parse(&argparse, argc, (const char **)argv);

	// if (arg_matrix) {
	// 	matrix_size = arg_matrix;
	// 	printf("Dithering ordonne %d\n", matrix_size);
	// 	floyd = 0;
	// }

	// if (arg_floyd > 0 && arg_floyd <= 10) {
	// 	printf("Dithering Floyd Steinberg - utilisation de la matrice %s\n", floyd_matrix[arg_floyd - 1].name);
	// 	floyd = 1;
	// 	matrix_size = 0;
	// } else if (arg_floyd > 10) {
	// 	printf("Dithering Floyd Steinberg - utilisation de la matrice standard\n", arg_floyd);
	// 	floyd = 1;
	// 	matrix_size = 0;
	// }


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


	if (the_image->width > 320) {
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
		IMAGE *new_image = bilinear_resize(the_image,
						   (int)the_image->width / ratio,
						   (int)the_image->height / ratio);
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




	if (the_conf->has_4096_colors) {
		// Dans ce cas, il faut chercher une palette
		if (arg_median_cut) {
			printf("Reduction de palette median-cut\n");
			guess_palette_median_cut(the_image, &the_palette, iter_median_cut);
		} else if (arg_octree) {
			printf("Reduction de palette octree\n");
			guess_palette_octree(the_image, &the_palette, 16);
		} else {
			printf("Reduction de palette wu\n");
			guess_palette_wu(the_image, &the_palette, 16);
		}
		thomson_post_trt_palette(&the_palette, &the_palette);
	} else {
		// Palette fixe de l'ordinateur
		printf("Palette fixe %d couleurs\n", the_conf->colors_count);
		load_palette(&the_palette, the_conf->palette, the_conf->colors_count);
	}

	fflush(stdout);


	/*
	 * for (int i = 0; i < 16; i++) {
	 *      if (i%2 == 0) {
	 *              the_palette.colors[i][0] = 0;
	 *              the_palette.colors[i][1] = 0;
	 *              the_palette.colors[i][2] = 0;
	 *      } else {
	 *              the_palette.colors[i][0] = 255;
	 *              the_palette.colors[i][1] = 255;
	 *              the_palette.colors[i][2] = 255;
	 *      }
	 * }
	 */


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

	MAP_40 map_40;

	init_map_40(&map_40);

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
	free_map_40(&map_40);

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

	SDL_SetWindowTitle(window, "Espace pour passer en mode color clash");

	message_loop();

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
