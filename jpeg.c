#include <stdio.h>
#include <stdlib.h>
#include <jpeglib.h>
#include "log.h"
#include "pixel.h"
#include "jpeg.h"



#define max(a, b)    (((a) > (b)) ? (a) : (b))
#define min(a, b)    (((a) < (b)) ? (a) : (b))




void free_image(IMAGE *image)
{
	if (image) {
		if (image->pixels)
			free(image->pixels);
		free(image);
	}
}



IMAGE *load(char *filename)
{
	/* these are standard libjpeg structures for reading(decompression) */
	struct jpeg_decompress_struct cinfo;
	struct jpeg_error_mgr jerr;
	/* libjpeg data structure for storing one row, that is, scanline of an image */
	JSAMPROW row_pointer[1];

	FILE *infile = fopen(filename, "rb");
	unsigned long location = 0;
	int i = 0;

	if (!infile) {
		printf("Error opening jpeg file %s\n!", filename);
		return NULL;
	}
	/* here we set up the standard libjpeg error handler */
	cinfo.err = jpeg_std_error(&jerr);
	/* setup decompression process and source, then read JPEG header */
	jpeg_create_decompress(&cinfo);
	/* this makes the library read from infile */
	jpeg_stdio_src(&cinfo, infile);
	/* reading the image header which contains image information */
	jpeg_read_header(&cinfo, TRUE);

	printf("Taille de l'image JPEG %d pixels * %d pixels\n", cinfo.image_width, cinfo.image_height);
	printf("Composantes par pixel %d\n", cinfo.num_components);
	printf("Espace couleur %d\n", cinfo.jpeg_color_space);

	/* Start decompression jpeg here */
	jpeg_start_decompress(&cinfo);

	/* now actually read the jpeg into the raw buffer */
	row_pointer[0] = (unsigned char *)malloc(cinfo.output_width * cinfo.num_components);

	IMAGE *image = (IMAGE *)malloc(sizeof(IMAGE));
	PIXEL *pixels = (PIXEL *)malloc(sizeof(PIXEL) * cinfo.image_width * cinfo.image_height);

	/* read one scan line at a time */
	while (cinfo.output_scanline < cinfo.image_height) {
		jpeg_read_scanlines(&cinfo, row_pointer, 1);

		for (int x = 0; x < cinfo.image_width; x++) {
			pixels[(cinfo.output_scanline - 1) * cinfo.image_width + x].r = row_pointer[0][x * 3];
			pixels[(cinfo.output_scanline - 1) * cinfo.image_width + x].g = row_pointer[0][x * 3 + 1];
			pixels[(cinfo.output_scanline - 1) * cinfo.image_width + x].b = row_pointer[0][x * 3 + 2];
		}
	}

	/* wrap up decompression, destroy objects, free pointers and close open files */
	jpeg_finish_decompress(&cinfo);
	jpeg_destroy_decompress(&cinfo);
	free(row_pointer[0]);
	fclose(infile);


	image->width = cinfo.output_width;
	image->height = cinfo.output_height;
	image->pixels = pixels;

	return image;
}
