#include "ordereddither.h"
#include "maputil.h"
#include <float.h>
#include <map.h>
#include <string.h>
#include <math.h>


void get_precalculated_matrix(short *matrix, int *matrix_size,
			      float *pre_calc_matrix)
{
	for (int i = 0; i < matrix_size[1] * matrix_size[0]; i++)
		pre_calc_matrix[i] = (matrix[i] + 0.0f) / (matrix_size[0] * matrix_size[1]);
}


IMAGE *ordered_dither(IMAGE *source, PALETTE *palette, float *matrix, int *matrix_size)
{
	PIXEL d;
	PIXEL_SHORT d2;
	RGB_COLOR c;
	unsigned char threshold[3];

	short index_color;
	float map_value;
	short percent, last_percent;

	// Seuil optimal par rapport a la palette
	// find_threshold(palette, threshold);

	threshold[0] = 32; threshold[1] = 32; threshold[2] = 32;

	printf("%d, %d, %d\n", threshold[0], threshold[1], threshold[2]);

	printf("14 REM Seuil optimal %u %u %u\n", threshold[0], threshold[1], threshold[2]);


	IMAGE *target = create_empty_image(source->width, source->height);

	if (target == NULL)
		fprintf(stderr, "ordered_dither - impossible de créer l'image");

	for (short y = 0; y < source->height; y++) {
		for (short x = 0; x < source->width; x++) {
			PIXEL p = source->pixels[y * source->width + x];

			if (matrix) {
				map_value = matrix[((y % matrix_size[1]) * matrix_size[0]) + (x % matrix_size[0])];

				d2.r = p.r + map_value * threshold[0];
				d2.g = p.g + map_value * threshold[1];
				d2.b = p.b + map_value * threshold[2];

				convert_pixel(&d2, &d);
			} else {
				d.r = p.r;
				d.g = p.g;
				d.b = p.b;
			}

			/*
			 * find_closest_color_rgb(&d, palette, &c);
			 * target->pixels[y * source->width + x].r = c[0];
			 * target->pixels[y * source->width + x].g = c[1];
			 * target->pixels[y * source->width + x].b = c[2];
			 */

			// Le rapprochement avec la palette est fait à la conversion IMAGE->*pixel
			// cf create_pixels_array
			target->pixels[y * source->width + x].r = d.r;
			target->pixels[y * source->width + x].g = d.g;
			target->pixels[y * source->width + x].b = d.b;
		}
	}

	return target;
}


float psycho_visual_color_compare(unsigned char c1[3], unsigned char c2[3])
{
	float luma1 = (c1[0] * 299 + c1[1] * 587 + c1[2] * 114) / (255.0 * 1000);
	float luma2 = (c2[0] * 299 + c2[1] * 587 + c2[2] * 114) / (255.0 * 1000);
	float lumadiff = luma1 - luma2;
	float diffR = (c1[0] - c2[0]) / 255.0, diffG = (c1[1] - c2[1]) / 255.0, diffB = (c1[2] - c2[2]) / 255.0;

	return (diffR * diffR * 0.299 + diffG * diffG * 0.587 + diffB * diffB * 0.114) * 0.75
	       + lumadiff * lumadiff;
}



MIXING_PLAN devise_best_mixing_plan(PIXEL *p, PALETTE *palette, int matrix_size)
{
	unsigned char c1[3]; c1[0] = p->r; c1[1] = p->g; c1[2] = p->b;
	unsigned char c2[3];
	MIXING_PLAN result = { { 0, 0 }, 0.5 };
	float least_penalty = FLT_MAX;

	// Loop through every unique combination of two colors from the palette,
	// and through each possible way to mix those two colors. They can be
	// mixed in exactly 64 ways, when the threshold matrix is 8x8.
	for (unsigned int index1 = 0; index1 < palette->size; ++index1)
		for (unsigned int index2 = index1; index2 < palette->size; ++index2)
			for (unsigned int ratio = 0; ratio < matrix_size; ++ratio) {
				if (index1 == index2 && ratio != 0) break;
				// Determine the two component colors
				unsigned int r1 = palette->colors[index1][0], g1 = palette->colors[index1][1], b1 = palette->colors[index1][2];
				unsigned int r2 = palette->colors[index2][0], g2 = palette->colors[index2][1], b2 = palette->colors[index2][2];
				// Determine what mixing them in this proportion will produce
				c2[0] = r1 + (ratio * (r2 - r1)) / (float)matrix_size;
				c2[1] = g1 + (ratio * (g2 - g1)) / (float)matrix_size;
				c2[2] = b1 + (ratio * (b2 - b1)) / (float)matrix_size;
				// Determine how well that matches what we want to accomplish
				float penalty = color_delta_f(c1, c2) + color_delta_f(palette->colors[index1], palette->colors[index2]) * 0.1;

				// float penalty = psycho_visual_color_compare(c1, c2)
				//  	+ psycho_visual_color_compare(palette->colors[index1], palette->colors[index2]) * 0.1;
				// float penalty = psycho_visual_color_compare(c1, c2)
				// 	+ psycho_visual_color_compare(palette->colors[index1], palette->colors[index2]) * 0.1
				// 	* (fabs(ratio-0.5)+0.5);

				if (penalty < least_penalty) {
					// Keep the result that has the smallest error
					least_penalty = penalty;
					result.colors[0] = index1;
					result.colors[1] = index2;
					result.ratio = ratio / (float)matrix_size;
				}
			}
	return result;
}

IMAGE *ordered_dither_yliluoma(IMAGE *source, PALETTE *palette, float *matrix, int *matrix_size)
{
	PIXEL d;
	PIXEL_SHORT d2;
	float map_value;
	MIXING_PLAN plan;
	char color_key[256];
	map processed_mixing_plan = map_init(9 + 1, sizeof(MIXING_PLAN), compare_c_string);

	IMAGE *target = create_empty_image(source->width, source->height);

	if (target == NULL)
		fprintf(stderr, "ordered_dither - impossible de créer l'image");

	int count = 0, hit = 0;

	if (matrix) {
		printf("Dithering ordonné façon Yliluoma ");
		fflush(stdout);
	}
	map_clear(processed_mixing_plan);
	for (short y = 0; y < source->height; y++) {
		int step = (source->height / 10) == 0 ? 1 : (source->height / 10);
		if (matrix && y % step == 0) {
			fflush(stdout);
			printf(".");
		}

		for (short x = 0; x < source->width; x++) {
			PIXEL p = source->pixels[y * source->width + x];

			if (matrix) {
				// map_value = matrix[((y % matrix_size[1]) * matrix_size[1]) + (x % matrix_size[0])];
				map_value = matrix[((y % matrix_size[1]) * matrix_size[0]) + (x % matrix_size[0])];

				// printf("%d %d - %d\n", x, y, ((y % matrix_size[1]) * matrix_size[0]) + (x % matrix_size[0]));

				sprintf(color_key, "%03d%03d%03d", p.r, p.g, p.b);
				bk_bool contained = map_get(&plan, processed_mixing_plan, color_key);
				if (!contained) {
					plan = devise_best_mixing_plan(&p, palette, matrix_size[0] * matrix_size[1]);
					map_put(processed_mixing_plan, color_key, &plan);
				} else {
					hit++;
				}

				count++;

				if (map_value < plan.ratio) {
					// printf("p2\n");
					d.r = palette->colors[plan.colors[1]][0];
					d.g = palette->colors[plan.colors[1]][1];
					d.b = palette->colors[plan.colors[1]][2];
				} else {
					// printf("p1\n");
					d.r = palette->colors[plan.colors[0]][0];
					d.g = palette->colors[plan.colors[0]][1];
					d.b = palette->colors[plan.colors[0]][2];
				}
			} else {
				d.r = p.r;
				d.g = p.g;
				d.b = p.b;
			}


			// Le rapprochement avec la palette est fait à la conversion IMAGE->*pixel
			// cf create_pixels_array
			target->pixels[y * source->width + x].r = d.r;
			target->pixels[y * source->width + x].g = d.g;
			target->pixels[y * source->width + x].b = d.b;
		}
	}

	// printf("map size:%d/%d hit:%d\n", map_size(processed_mixing_plan), count, hit);
	map_destroy(processed_mixing_plan);
	if (matrix) {
		printf("\n");
		fflush(stdout);
	}

	return target;
}
