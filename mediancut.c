#include "mediancut.h"
#include "maputil.h"
// #include "pixel.h"
#include <math.h>
#include <map.h>





void init_bucket(BUCKET *bucket, unsigned int size)
{
	bucket->colors = malloc(size * sizeof(char *));
	for (unsigned int i = 0; i < size; i++)
		bucket->colors[i] = malloc(3);
	bucket->size = size;
}


void free_bucket(BUCKET *bucket)
{
	if (bucket->size > 0) {
		for (int i = 0; i < bucket->size; i++)
			free(bucket->colors[i]);
		free(bucket->colors);
	}
}

void swap_colors(BUCKET *bucket, int i, int j)
{
	unsigned char tmp[3];

	tmp[0] = bucket->colors[j][0];
	tmp[1] = bucket->colors[j][1];
	tmp[2] = bucket->colors[j][2];

	bucket->colors[j][0] = bucket->colors[i][0];
	bucket->colors[j][1] = bucket->colors[i][1];
	bucket->colors[j][2] = bucket->colors[i][2];

	bucket->colors[i][0] = tmp[0];
	bucket->colors[i][1] = tmp[1];
	bucket->colors[i][2] = tmp[2];
}

int partition_bucket(BUCKET *bucket, int range, int low, int high)
{
	int pivot = bucket->colors[high][range];        // pivot
	int i = (low - 1);                              // Index of smaller element

	for (int j = low; j <= high - 1; j++) {
		// If current element is smaller than the pivot
		if (bucket->colors[j][range] < pivot) {
			i++; // increment index of smaller element
			swap_colors(bucket, i, j);
		}
	}
	swap_colors(bucket, i + 1, high);
	return i + 1;
}

// Tri d'un bucket en quickSort
void sort_bucket(BUCKET *bucket, int range, int low, int high)
{
	if (low < high) {
		/* pi is partitioning index, arr[p] is now
		 * at right place */
		int pi = partition_bucket(bucket, range, low, high);

		// Separately sort elements before
		// partition and after partition
		sort_bucket(bucket, range, low, pi - 1);
		sort_bucket(bucket, range, pi + 1, high);
	}
}

int find_greatest_range(BUCKET *bucket)
{
	unsigned char r = 0, g = 0, b = 0;

	for (int i = 0; i < bucket->size; i++) {
		if (bucket->colors[i][0] > r)
			r = bucket->colors[i][0];
		if (bucket->colors[i][1] > g)
			g = bucket->colors[i][1];
		if (bucket->colors[i][2] > b)
			b = bucket->colors[i][2];
	}

	// printf("gr %d   gg %d   gb %d\n", r, g, b);

	if (r >= g && r >= b)
		return 0;
	else if (g >= r && g >= b)
		return 1;
	else
		return 2;
}

void split_bucket(BUCKET *bucket, BUCKET *new_bucket)
{
	int mid = bucket->size / 2;

	init_bucket(new_bucket, bucket->size - mid);
	for (int i = mid; i < bucket->size; i++) {
		new_bucket->colors[i - mid][0] = bucket->colors[i][0];
		new_bucket->colors[i - mid][1] = bucket->colors[i][1];
		new_bucket->colors[i - mid][2] = bucket->colors[i][2];

		free(bucket->colors[i]);
	}
	bucket->size = mid;
}

void mean_bucket(BUCKET *bucket)
{
	unsigned int sum_r = 0, sum_g = 0, sum_b = 0;

	for (int i = 0; i < bucket->size; i++) {
		sum_r += bucket->colors[i][0];
		sum_g += bucket->colors[i][1];
		sum_b += bucket->colors[i][2];
	}
	bucket->mean[0] = sum_r / bucket->size;
	bucket->mean[1] = sum_g / bucket->size;
	bucket->mean[2] = sum_b / bucket->size;
}

void print_bucket(BUCKET *bucket)
{
	printf("Taille : %d\n", bucket->size);
	for (int i = 0; i < bucket->size; i++)
		printf("(%u %u %u)\n", bucket->colors[i][0], bucket->colors[i][1], bucket->colors[i][2]);
}


void find_threshold(PALETTE *palette, unsigned char threshold[3])
{
	BUCKET bucket;
	unsigned int diff;
	int r, g, b;

	init_bucket(&bucket, palette->size);
	for (int i = 0; i < palette->size; i++) {
		// _16_to_256_comp(palette->colors[i][0], palette->colors[i][1], palette->colors[i][2], &r, &g, &b);
		/*
		 * bucket.colors[i][0] = r;
		 * bucket.colors[i][1] = g;
		 * bucket.colors[i][2] = b;
		 */

		bucket.colors[i][0] = palette->colors[i][0];
		bucket.colors[i][1] = palette->colors[i][1];
		bucket.colors[i][2] = palette->colors[i][2];
	}
	for (int k = 0; k < 3; k++) {
		threshold[k] = 0;
		sort_bucket(&bucket, k, 0, bucket.size - 1);
		for (int i = 0; i < palette->size - 1; i++) {
			// diff = my_abs(bucket.colors[i][k] - bucket.colors[i + 1][k]);
			diff = abs(bucket.colors[i][k] - bucket.colors[i + 1][k]);
			if (diff >= threshold[k])
				threshold[k] = diff;
		}
	}
}

void guess_palette_median_cut(IMAGE *image, PALETTE *palette, int iter)
{
	short max_colors = 1;

	for (int i = 0; i < iter; i++)
		max_colors *= 2;


	BUCKET bucket[max_colors];

	short full_palette[4096];
	short thomson_color;
	short colors_count = 0;
	unsigned int color_index = 0;
	unsigned char r, g, b;
	int r_16, g_16, b_16;

	for (int i = 0; i < 4096; i++)
		full_palette[i] = 0;


	// Recherche du nombre de couleurs 24 bits
	unsigned int color_index_24 = 0;
	map colors = map_init(sizeof(unsigned int), sizeof(unsigned int), compare_int);

	int count_24;

	for (int y = 0; y < image->height; y++) {
		for (int x = 0; x < image->width; x++) {
			color_index_24 = (image->pixels[x + y * image->width].r << 16)
					 + (image->pixels[x + y * image->width].g << 8)
					 + image->pixels[x + y * image->width].b;

			// printf("%d %d\n", image->pixels[x + y * image->width].r, color_index_24);
			int inside = map_get(&count_24, colors, &color_index_24);

			if (inside == BK_FALSE) {
				int v = 1;
				map_put(colors, &color_index_24, &v);
				// printf("ins %u\n", color_index_24);
			} else {
				// printf("get %u:%d\n", color_index_24, count_24);
				count_24++;
				map_put(colors, &color_index_24, &count_24);
			}
		}
	}
	// printf("map size:%d\n", map_size(colors));
	unsigned int *first_key, *last_key, *current_key;

	first_key = map_first(colors);
	last_key = map_last(colors);
	// printf("first lower:%u - last:%u\n", *first_key, *last_key);

	init_bucket(&bucket[0], map_size(colors));
	// printf("init\n");
	current_key = first_key;
	while (current_key != NULL) {
		map_get(&count_24, colors, current_key);
		// printf("get %u -> %u:%d\n", color_index, *current_key, count_24);

		r = (*current_key) / 65536;
		g = ((*current_key) % 65536) / 256;
		b = ((*current_key) % 65536) % 256;

		// printf("color_index %d ->  %d = %u %u %u\n", color_index, *current_key, r, g, b);
		bucket[0].colors[color_index][0] = r;
		bucket[0].colors[color_index][1] = g;
		bucket[0].colors[color_index][2] = b;

		current_key = map_higher(colors, current_key);
		color_index++;
	}

	map_destroy(colors);

	int last_bucket = 1;
	int bucket_count = 0;
	int new_idx = 0;
	int greatest_range = 0;

	for (int k = 0; k < iter; k++) {
		new_idx = last_bucket;
		for (int i = 0; i < last_bucket; i++) {
			greatest_range = find_greatest_range(&bucket[i]);
			sort_bucket(&bucket[i], greatest_range, 0, bucket[i].size - 1);
			split_bucket(&bucket[i], &bucket[new_idx]);
			new_idx++;
		}
		last_bucket *= 2;
	}


	for (int i = 0; i < max_colors; i++)
		mean_bucket(&bucket[i]);


	// palette
	for (int i = 0; i < max_colors; i++) {
		palette->colors[i][0] = bucket[i].mean[0];
		palette->colors[i][1] = bucket[i].mean[1];
		palette->colors[i][2] = bucket[i].mean[2];
		free_bucket(&bucket[i]);
	}
	palette->size = max_colors;
}
