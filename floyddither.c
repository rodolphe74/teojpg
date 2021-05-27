#include "floyddither.h"
#include <math.h>


#define max(a, b) (((a) > (b)) ? (a) : (b))
#define min(a, b) (((a) < (b)) ? (a) : (b))


void apply_error(PIXEL *p, float ratio, short error[3])
{
	short new_value;

	new_value = (*p).r + ratio * error[0];
	(*p).r = (int)min(max(new_value, 0), 255);
	new_value = (*p).g + ratio * error[1];
	(*p).g = (int)min(max(new_value, 0), 255);
	new_value = (*p).b + ratio * error[2];
	(*p).b = (int)min(max(new_value, 0), 255);
}



IMAGE *floyd_steinberg_dither(IMAGE *source, PALETTE *palette, FLOYD_MATRIX *f_mat)
{
	unsigned char old_pixel[3], new_pixel[3];
	short error_propagation[3];

	float *matrix = f_mat->matrix;
	int serpentine = 1;
	int left_to_right = 1;
	int matrix_size = (int)matrix[0];

	int serpentine_x = 0;
	int matrix_shift, yy;
	float error;

	IMAGE *target = create_empty_image(source->width, source->height);

	if (target == NULL)
		fprintf(stderr, "ordered_dither - impossible de créer l'image");


	for (int i = 0; i < source->width * source->height; i++)
		target->pixels[i] = source->pixels[i];


	for (int y = 0; y < target->height; y++) {
		for (int x = 0; x < target->width; x++) {
			if (serpentine) {
				if ((y + 1) % 2) {
					serpentine_x = x;
					left_to_right = 1;
				} else {
					serpentine_x = target->width - 1 - x;
					left_to_right = 0;
				}
			} else {
				serpentine_x = x;
			}

			old_pixel[0] = target->pixels[y * target->width + serpentine_x].r;
			old_pixel[1] = target->pixels[y * target->width + serpentine_x].g;
			old_pixel[2] = target->pixels[y * target->width + serpentine_x].b;

			PIXEL p;
			p.r = old_pixel[0];
			p.g = old_pixel[1];
			p.b = old_pixel[2];

			RGB_COLOR c;
			find_closest_color_rgb(&p, palette, &c);
			new_pixel[0] = c[0];
			new_pixel[1] = c[1];
			new_pixel[2] = c[2];

			target->pixels[y * target->width + serpentine_x].r = new_pixel[0];
			target->pixels[y * target->width + serpentine_x].g = new_pixel[1];
			target->pixels[y * target->width + serpentine_x].b = new_pixel[2];

			error_propagation[0] = old_pixel[0] - new_pixel[0];
			error_propagation[1] = old_pixel[1] - new_pixel[1];
			error_propagation[2] = old_pixel[2] - new_pixel[2];

			for (int i = 0; i < matrix_size; i++) {
				if (left_to_right)
					matrix_shift = (int)matrix[1 + (i % matrix_size) * 3];
				else
					matrix_shift = 0 - (int)matrix[1 + (i % matrix_size) * 3];
				yy = (int)matrix[1 + (i % matrix_size) * 3 + 1];
				error = matrix[1 + (i % matrix_size) * 3 + 2];

				if (serpentine_x + matrix_shift >= 0 && serpentine_x + matrix_shift < target->width && y + yy >= 0 && y + yy < target->height)
					apply_error(&target->pixels[(y + yy) * target->width + matrix_shift + serpentine_x], error, error_propagation);
			}
		}
	}
	return target;
}



IMAGE *ostromoukhov_dither(IMAGE *source, PALETTE *palette)
{
	unsigned char old_pixel[3], new_pixel[3];
	short error_propagation[3];

	int serpentine = 1;
	int left_to_right = 1;

	int serpentine_x = 0;
	int shift;
	float error;

	IMAGE *target = create_empty_image(source->width, source->height);

	if (target == NULL)
		fprintf(stderr, "ordered_dither - impossible de créer l'image");


	for (int i = 0; i < source->width * source->height; i++)
		target->pixels[i] = source->pixels[i];


	for (int y = 0; y < target->height; y++) {
		for (int x = 0; x < target->width; x++) {
			if (serpentine) {
				if ((y + 1) % 2) {
					serpentine_x = x;
					left_to_right = 1;
				} else {
					serpentine_x = target->width - 1 - x;
					left_to_right = 0;
				}
			} else {
				serpentine_x = x;
			}

			old_pixel[0] = target->pixels[y * target->width + serpentine_x].r;
			old_pixel[1] = target->pixels[y * target->width + serpentine_x].g;
			old_pixel[2] = target->pixels[y * target->width + serpentine_x].b;

			PIXEL p;
			p.r = old_pixel[0];
			p.g = old_pixel[1];
			p.b = old_pixel[2];

			RGB_COLOR c;
			find_closest_color_rgb(&p, palette, &c);
			new_pixel[0] = c[0];
			new_pixel[1] = c[1];
			new_pixel[2] = c[2];

			target->pixels[y * target->width + serpentine_x].r = new_pixel[0];
			target->pixels[y * target->width + serpentine_x].g = new_pixel[1];
			target->pixels[y * target->width + serpentine_x].b = new_pixel[2];

			error_propagation[0] = old_pixel[0] - new_pixel[0];
			error_propagation[1] = old_pixel[1] - new_pixel[1];
			error_propagation[2] = old_pixel[2] - new_pixel[2];

			// int intensity = old_pixel[0]/3 + old_pixel[1]/3 + old_pixel[2]/3;
			int intensity = (int)round(old_pixel[0] * 0.299 + 0.587 * old_pixel[1] + 0.114 * old_pixel[2]);

			OSTRO_COEFS oc = OSTRO_COEFS_ARRAY[intensity];

			if (left_to_right)
				shift = 1;
			else
				shift = -1;


			if (serpentine_x + shift >= 0 && serpentine_x + shift < target->width)
				apply_error(&target->pixels[(y + 0) * target->width + shift + serpentine_x], oc.i_r / (float)oc.i_sum, error_propagation);

			if (y + 1 < target->height - 1) {
				if (serpentine_x + shift >= 0 && serpentine_x + shift < target->width)
					apply_error(&target->pixels[(y + 1) * target->width - shift + serpentine_x], oc.i_dl / (float)oc.i_sum, error_propagation);
				apply_error(&target->pixels[(y + 1) * target->width + 0 + serpentine_x], oc.i_d / (float)oc.i_sum, error_propagation);
			}
		}
	}
	return target;
}
