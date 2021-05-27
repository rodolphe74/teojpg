#include "thomson.h"
#include "maputil.h"
#include "colourconverter.h"
#include <float.h>
#include <math.h>
#include <string.h>
#include <map.h>


unsigned int thomson_rvb[4096]; /* extern */
void init_thomson_rvb()
{
	int gamma4096[16] = { 0, 38, 48, 56, 62, 67, 72, 76, 80, 83, 86, 89, 92, 95, 98, 100 };
	int i = 16;

	for (int r = 0; r < 16; r++) for (int g = 0; g < 16; g++) for (int b = 0; b < 16; b++) {
				int x1, x2, x3;
				x1 = gamma4096[b] * 255 / gamma4096[15];        //rouge
				x2 = gamma4096[g] * 255 / gamma4096[15];        //vert
				x3 = gamma4096[r] * 255 / gamma4096[15];        //bleu
				thomson_rvb[i] = (x1 << 16) + (x2 << 8) + x3;
				i++;
			}
}


float thomson_linear_levels[16];        /* extern */
float thomson_levels[16];               /* extern */
int linear_to_to[256];                  /* extern */
int rgb_to_to[256];                     /* extern */
void init_thomson_linear_levels()
{
	for (int i = 0; i < 16; i++) {
		thomson_linear_levels[i] = linear_space_f(thomson_level_pc[i]);

		// printf(" ##  thomson_linear_levels[%d]  %f   %d\n", i, thomson_linear_levels[i], thomson_level_pc[i]);

		thomson_levels[i] = thomson_level_pc[i];
	}

	// for (int i = 0; i < 16; i++)
	// 	thomson_linear_levels[i] = thomson_level_pc[i];


	float r;
	float dm = FLT_MAX;
	int cm = 0;

	for (int i = 0; i < 256; i++) {
		dm = FLT_MAX;
		r = i;
		for (int c = 0; c < 16; c++) {
			float d = fabs(thomson_linear_levels[c] - r);
			if (d < dm) {
				cm = c;
				dm = d;
			}
		}
		linear_to_to[i] = cm;
	}

	cm = 0;
	for (int i = 0; i < 256; i++) {
		dm = FLT_MAX;
		r = i;
		for (int c = 0; c < 16; c++) {
			float d = fabs(thomson_levels[c] - r);

			// printf("    %f < %f\n", d, dm);

			if (d < dm) {
				cm = c;
				dm = d;
			}
		}
		rgb_to_to[i] = cm;
	}
}


int get_palette_thomson_value(int r, int g, int b)
{
	int r_16 = rgb_to_to[r];
	int g_16 = rgb_to_to[g];
	int b_16 = rgb_to_to[b];

	/*
	 * printf("_(%d %d %d) -> (%d %d %d) = %d\n", r, g, b, r_16, g_16, b_16,
	 *      (r_16 + 1) + (g_16 + 1) * 16 + (b_16 + 1) * 256 - 273);
	 */

	return (r_16 + 1) + (g_16 + 1) * 16 + (b_16 + 1) * 256 - 273;
}


int get_index_color_thomson_to(int back_index, int fore_index)
{
	// Palette thomson TO xyBVRBVR | x = 0 : fd pastel | y = 0 fo pastel
	// N,R,V,J,B,M,C,BL (fonce)
	// x,x,x,x,x,x,x,OR (pastel)

	// couleur > 7 = pastel
	int subst_back = (back_index > 7 ? 8 : 0);
	int subst_fore = (fore_index > 7 ? 8 : 0);
	unsigned char idx = (back_index > 7 ? 0 : 1) << 7
			    | (fore_index > 7 ? 0 : 1) << 6
			    | (fore_index - subst_fore) << 3
			    | (back_index - subst_back);

	return idx;
}


int get_index_color_thomson_mo(int back_index, int fore_index)
{
	// Palette thomson MO5/6 xBVRyBVR | x = 1 : fd pastel | y = 1 fo pastel
	// N,R,V,J,B,M,C,BL (fonce)
	// x,x,x,x,x,x,x,OR (pastel)

	// couleur > 7 = pastel
	int subst_back = (back_index > 7 ? 8 : 0);
	int subst_fore = (fore_index > 7 ? 8 : 0);
	unsigned char idx = (fore_index > 7 ? 1 : 0) << 7
			    | (fore_index) << 4
			    | (back_index > 7 ? 1 : 0) << 3
			    | (back_index);

	return idx;
}


void dec_to_binary(int n, char *binaryNum)
{
	int i = 0;

	while (n > 0) {
		binaryNum[i] = (n % 2 == 0 ? '0' : '1');
		n = n / 2;
		i++;
	}
}


int count_colors_32(IMAGE *image, PALETTE *palette)
{
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
	int color_index = 0;

	palette->size = map_size(colors);
	current_key = first_key;
	while (current_key != NULL) {
		map_get(&count_24, colors, current_key);
		// printf("get %u -> %u:%d\n", color_index, *current_key, count_24);

		palette->colors[color_index][0] = (*current_key) / 65536;
		palette->colors[color_index][1] = ((*current_key) % 65536) / 256;
		palette->colors[color_index][2] = ((*current_key) % 65536) % 256;

		current_key = map_higher(colors, current_key);
		color_index++;
	}

	map_destroy(colors);

	return palette->size;
}



float compare_blocs_f(unsigned char src_bloc[8], unsigned char cmp_bloc[8], unsigned char prev_bloc[8],
	int first_bloc, PALETTE *palette)
{
	float delta = 0.;

	for (int i = 0; i < 8; i++)
			delta += color_delta_f(palette->colors[src_bloc[i]], palette->colors[cmp_bloc[i]]);

	// if (first_bloc) {
	// 	for (int i = 0; i < 8; i++)
	// 		delta += color_delta_f(palette->colors[src_bloc[i]], palette->colors[cmp_bloc[i]]);
	// } else {
	// 	// On va essayer d'inclure le bloc précédent dans le calcul du delta
	// 	int c1_prev, c2_prev;
	// 	c1_prev = prev_bloc[0];
	// 	c2_prev = c1_prev;
	// 	for (int i = 1; i < 8; i++)
	// 		if (prev_bloc[i] != c1_prev) {
	// 			c2_prev = prev_bloc[i];
	// 			break;
	// 		}
	// 	// printf("Couleurs précédentes:%d %d\n", c1_prev, c2_prev);

	// 	for (int i = 0; i < 8; i++) {
	// 		delta += color_delta_f(palette->colors[src_bloc[i]], palette->colors[cmp_bloc[i]]);
	// 		// printf("delta %f", delta);
	// 		if (cmp_bloc[i] == c1_prev /*|| cmp_bloc[i] == c2_prev*/) {
	// 			// printf("bonus");
	// 			delta -= 500;
	// 		}
	// 		// printf("> %f\n", delta);
	// 	}
	// }

	return delta;
}


void convert_bloc_to_thomson(unsigned char bloc[8], unsigned char thomson_bloc[3])
{
	// Conversion du bloc en valeur thomson to/mo
	// en sortie :
	// thomson_bloc[0] = forme
	// thomson_bloc[1] = couleurs format TO
	// thomson_bloc[2] = couleurs format MO
	// En basic, le format de la oouleur est spécifié en fonction de la config TO/MO
	// En SNAP-TO, le format de la couleur est toujours TO

	// recherche des couleurs
	int fd = bloc[0];
	int fo = -1;
	int val = 0, coul = 0;

	for (int i = 0; i < 8; i++)
		if (bloc[i] != fd)
			fo = bloc[i];

	// Calcul forme
	for (int i = 7; i >= 0; i--)
		if (bloc[i] == fo)
			val += pow(2, i);

	// Couleur MO / TO
	thomson_bloc[1] = get_index_color_thomson_to(fd, fo <= 0 ? 0 : fo);
	thomson_bloc[2] = get_index_color_thomson_mo(fd, fo <= 0 ? 0 : fo);

	thomson_bloc[0] = val;
}



void thomson_post_trt_palette(PALETTE *src, PALETTE *target)
{
	// Les teintes sombres sont modifiées
	// pour améliorer la correspondance Thomson
	// Ajout du noir pour améliorer les contrastes dans le tramage
	HSL d;
	RGBdec r;

	d.L = 100;
	int min = -1;

	for (int i = 0; i < src->size; i++) {
		RGBdec c;
		HSL h;
		c.R = src->colors[i][0];
		c.G = src->colors[i][1];
		c.B = src->colors[i][2];
		dec2hsl(c, &h);

		if (h.L < 50) {
			if (h.L < d.L) {
				d.H = h.H;
				d.L = h.L;
				d.S = h.S;
				min = i;
			}
			h.L = 50;       // boost la lum à 50
			hsl2dec(h, &c);
		}

		if (min >= 0) {
			target->colors[i][0] = c.R;
			target->colors[i][1] = c.G;
			target->colors[i][2] = c.B;
		}
	}

	target->colors[min][0] = 0;
	target->colors[min][1] = 0;
	target->colors[min][2] = 0;
}




unsigned char *thomson_post_trt_combin(IMAGE *source, PALETTE *palette, MAP_40 *map_40,
				       MACHINE_CONF *conf, char *target_name)
{
	char basic_filename[256];

	sprintf(basic_filename, "%s.bas", target_name);
	FILE *basic_out = fopen(basic_filename, "w");
	char str_value[16];
	char data_values[512];
	char data_line[512];

	// decoupage bloc 8 pixels
	// couleur optimale sur bloc 8
	unsigned char *pixels = malloc(sizeof(unsigned char) * source->height * source->width);

	memset(pixels, 0, source->height * source->width);

	if (!pixels) {
		printf("Pas assez de memoire\n");
		return NULL;
	}

	IMAGE *guess_8_image = (IMAGE *)malloc(sizeof(IMAGE));
	PIXEL *guess_8_pixels = (PIXEL *)malloc(sizeof(PIXEL) * 8 * 1);
	PALETTE guess_8_palette;


	int count = 0, last_idx = 0;
	char binary_string[9];
	char byte[9];
	unsigned char src_bloc[8];
	unsigned char cmp_bloc[8];
	unsigned char prev_bloc[8];
	unsigned char best_bloc[8];

	// Thomson TO : Mémoire écran &h4000 à &h5f40 exclu
	int start_adress = 0x4000;
	// int start_adress = 0x0000;
	int start_line = 1000;
	char line[8192] = { 0 };
	char bloc[512] = { 0 };

	int r, g, b;

	int map_column, map_line;

	fprintf(basic_out, "1 CLS\n");
	fprintf(basic_out, "2 L=%d:C=%d:S=&H%04X:E=S\n", source->height, source->width / 8, conf->start_screen_address);

	if (conf->has_4096_colors) {
		// Dans ce cas, ecriture de la palette
		for (int i = 0; i < palette->size; i++) {
			r = palette->colors[i][0];
			g = palette->colors[i][1];
			b = palette->colors[i][2];
			fprintf(basic_out, "%d PALETTE %d,%d\n", i + 3, i, get_palette_thomson_value(r, g, b));
		}
	}

	fprintf(basic_out, "21 FOR Y = 1 TO L\n");
	fprintf(basic_out, "30 FOR X = 1 TO C\n");
	fprintf(basic_out, "31 READ A\n");
	fprintf(basic_out, "32 GOSUB 200\n");
	fprintf(basic_out, "33 REM PRINT E,B1,B2\n");
	fprintf(basic_out, "35 GOSUB 300\n");
	fprintf(basic_out, "39 E=E+1\n");
	fprintf(basic_out, "40 NEXT X\n");
	fprintf(basic_out, "41 E=S+40*Y\n");
	fprintf(basic_out, "45 NEXT Y\n");
	fprintf(basic_out, "50 END\n");
	fprintf(basic_out, "200 B=A\n");
	fprintf(basic_out, "210 B1=INT(B/256)\n");
	fprintf(basic_out, "220 B2=B-(B1*256)\n");
	fprintf(basic_out, "230 RETURN\n");
	fprintf(basic_out, "300 Z=PEEK(&H%04X) AND 254\n", conf->commut_address);
	fprintf(basic_out, "310 POKE &H%04X,Z\n", conf->commut_address);
	fprintf(basic_out, "320 POKE E,B2\n");
	fprintf(basic_out, "330 Z=PEEK(&H%04X) OR 1\n", conf->commut_address);
	fprintf(basic_out, "340 POKE &H%04X,Z\n", conf->commut_address);
	fprintf(basic_out, "350 POKE E,B1\n");
	fprintf(basic_out, "360 RETURN\n");


	short x, y;

	printf("Traitement des combinatoires color clash ");
	for (y = 0; y < source->height; y++) {
		int step = (source->height / 10) == 0 ? 1 : (source->height / 10);
		if (y % step == 0) {
			fflush(stdout);
			printf(".");
		}


		count = 0;

		// DEBUG
		memset(data_values, 0, 512);

		for (x = 0; x < source->width; x++) {
			PIXEL p = source->pixels[y * source->width + x];

			if (count == 7) {
				// cas standard 8 pixels
				for (short i = /*7*/ count; i >= 0; i--) {
					guess_8_pixels[i].r = source->pixels[y * source->width + x - i].r;
					guess_8_pixels[i].g = source->pixels[y * source->width + x - i].g;
					guess_8_pixels[i].b = source->pixels[y * source->width + x - i].b;
					last_idx = y * source->width + x - i;
					src_bloc[i] = find_closest_color_index(&guess_8_pixels[i], palette);
				}


				guess_8_image->width = 8;
				guess_8_image->height = 1;
				guess_8_image->pixels = guess_8_pixels;


				count_colors_32(guess_8_image, &guess_8_palette);

				// exploration des combinatoires
				int comb = 0;
				int color_1, color_2;
				int index_color_1, index_color_2;
				// int best_delta = INT_MAX;
				float best_delta = FLT_MAX;
				PIXEL pix_1, pix_2;

				for (int i = 0; i < guess_8_palette.size; i++) {
					pix_1.r = guess_8_palette.colors[i][0];
					pix_1.g = guess_8_palette.colors[i][1];
					pix_1.b = guess_8_palette.colors[i][2];

					index_color_1 = find_closest_color_index(&pix_1, palette);

					for (int j = 0; j < guess_8_palette.size; j++) {
						pix_2.r = guess_8_palette.colors[j][0];
						pix_2.g = guess_8_palette.colors[j][1];
						pix_2.b = guess_8_palette.colors[j][2];

						index_color_2 = find_closest_color_index(&pix_2, palette);

						for (int k = 0; k < 256; k++) {
							// itoa(k, binary_string, 2);
							dec_to_binary(k, binary_string);
							sprintf(byte, "%*.*s%s", 8 - strlen(binary_string), 8 - strlen(binary_string), "00000000", binary_string);

							for (int l = 0; l < 8; l++)
								cmp_bloc[l] = (byte[l] == '0' ? index_color_1 : index_color_2);

							// Comparaison : les tableaux contiennent les indexes de palette (source et la combinaison générée en cours)
							// Comment faire pour éviter au mieux les effets de colors clash ?
							// Pour l'instant : somme des différences entre chaque couleur du bloc de 8 pixels et on prend la distance
							// la plus courte.
							// Le bloc précédent au bloc courant est passé en paramètre. Je ne sais pas encore comment l'utiliser.
							float delta = compare_blocs_f(src_bloc, cmp_bloc, prev_bloc, x < 8 ? 1 : 0, palette);
							if (delta < best_delta) {
								best_delta = delta;
								memcpy(best_bloc, cmp_bloc, 8);
							}

							// Sauvegarde du bloc précédent
							memcpy(prev_bloc, best_bloc, 8);

							comb++;
						}
					}
				}

				int value = 0;
				for (short i = /*7*/ count; i >= 0; i--)
					pixels[y * source->width + x - i] = best_bloc[i];
				unsigned char ret[3];
				convert_bloc_to_thomson(best_bloc, ret);

				if (conf->start_screen_address == 0x4000)
					sprintf(str_value, "%d,", ret[0] * 256 + ret[1]);
				else
					sprintf(str_value, "%d,", ret[0] * 256 + ret[2]);
				strcat(data_values, str_value);

				list_add_last(map_40->rama, &ret[0]);
				list_add_last(map_40->ramb, &ret[1]);
				start_line++;
				count = -1;
			}
			count++;
		}

		// Nouvelle ligne
		start_adress += 40;
		start_line += 10;

		data_values[strlen(data_values) - 1] = 0; // sup virgule
		sprintf(data_line, "%d DATA %s\n", start_line, data_values);
		fprintf(basic_out, data_line);
	}
	printf("\nLe fichier basic est créé\n");
	fflush(stdout);

	free(guess_8_pixels);
	free(guess_8_image);

	map_40->lines = y;
	map_40->columns = x / 8;

	fclose(basic_out);

	return pixels;
}







void init_map_40(MAP_40 *map_40)
{
	map_40->rama = list_init(sizeof(unsigned char));
	map_40->ramb = list_init(sizeof(unsigned char));
}

void free_map_40(MAP_40 *map_40)
{
	list_destroy(map_40->rama);
	list_destroy(map_40->ramb);
}


int read_ahead(list *buffer_list, int idx)
{
	unsigned char current;
	unsigned char compare_to;

	list_get_at(&compare_to, *buffer_list, idx);
	int repeat = 0;

	for (int i = idx + 1; i < list_size(*buffer_list); i++) {
		list_get_at(&current, *buffer_list, i);
		if (compare_to != current || repeat > 253)
			break;
		repeat++;
	}
	return repeat;
}


void write_segment(list *target, FILE *f, list *buffer_list, int i, unsigned char seg_size)
{
	unsigned char current;
	unsigned char header[2];

	header[0] = 0;
	header[1] = seg_size;

	list_add_last(*target, &header[0]);
	list_add_last(*target, &header[1]);
	for (int j = i - seg_size; j < i; j++) {
		list_get_at(&current, *buffer_list, j);
		list_add_last(*target, &current);
	}
}

void compress(list *target, FILE *f, list *buffer_list)
{
	// Traitement du buffer;
	int i = 0;
	int seg = 0;
	unsigned char current;

	while (i < list_size(*buffer_list)) {
		int repeat = read_ahead(buffer_list, i);

		if (repeat == 0) {
			i += 1;
			seg++;

			if (seg > 254) {
				write_segment(target, f, buffer_list, i, seg);
				seg = 0;
			}
		} else {
			if (seg > 0)
				write_segment(target, f, buffer_list, i, (unsigned char)seg);

			i += (repeat + 1);
			seg = 0;

			unsigned char rep_count;
			rep_count = repeat + 1;
			list_add_last(*target, &rep_count);
			list_get_at(&current, *buffer_list, i - repeat - 1);
			list_add_last(*target, &current);
		}
	}

	// flush
	if (seg > 0)
		write_segment(target, f, buffer_list, i, seg);

	// cloture
	unsigned char cloture[2] = { 0, 0 };

	list_add_last(*target, &cloture[0]);
	list_add_last(*target, &cloture[1]);
}


void transpose_data_map_40(int columns, int lines, list *src, list *target)
{
	unsigned char current;
	unsigned char zero = 0;

	// Le nombre de lignes doit être un multiple de 8
	// La hauteur est inscrite dans l'entêtte du map
	// sous la forme (map_40->lines - 1) / 8

	int padding = lines % 8;
	int add_line = 8 - padding;

	for (int x = 0; x < columns; x++) {
		for (int y = 0; y < lines; y++) {
			list_get_at(&current, *src, y * columns + x);
			// if (y == 1 && x > 30) {
			// 	printf("transpose (%d %d) = %d\n", x, current, list_size(*target));
			// }
			list_add_last(*target, &current);
		}

		if (padding)
			for (int y = 0; y < add_line; y++)
				list_add_last(*target, &zero);

		add_line = 8 - padding;
	}
}

void save_map_40_col(char *filename, MAP_40 *map_40, PALETTE *palette)
{
	list buffer_list = list_init(sizeof(unsigned char));
	int r, g, b;

	unsigned char current;

	// DEBUG
	// unsigned char b[] = { 0x00, 0x7f, 0x40, 0x40, 0x40, 0x40, 0x7f, 0x00, 0x00, 0xFE, 0x02, 0x02, 0x02, 0x02, 0xFE, 0x00 };
	// for (int i = 0; i < 16; i++) {
	//     current = b[i];
	//     list_add_last(buffer_list, &current);
	// }
	////////////////////


	FILE *fout;
	list target_buffer_list = list_init(sizeof(unsigned char));

	char map_filename[256];

	sprintf(map_filename, "%s.map", filename);
	if ((fout = fopen(map_filename, "wb")) == NULL) {
		fprintf(stderr, "Impossible d'ouvrir le fichier données en écriture\n");
		return;
	}

	transpose_data_map_40(map_40->columns, map_40->lines, &map_40->rama, &buffer_list);
	compress(&target_buffer_list, fout, &buffer_list);

	list_clear(buffer_list);

	// DEBUG
	// unsigned char c[] = { 0xC8, 0xC8, 0xC8, 0xC8, 0xC8, 0xC8, 0xC8, 0xC8, 0xC8, 0xC8, 0xC8, 0xC8, 0xC8, 0xC8, 0xC8, 0xC8 };
	// for (int i = 0; i < 16; i++) {
	//     current = c[i];
	//     list_add_last(buffer_list, &current);
	// }
	////////////////////

	transpose_data_map_40(map_40->columns, map_40->lines, &map_40->ramb, &buffer_list);
	compress(&target_buffer_list, fout, &buffer_list);

	// Ecriture de l'entete
	// unsigned short size = (unsigned short) list_size(target_buffer_list) + 3;
	unsigned short size = (unsigned short)list_size(target_buffer_list) + 3 + 40;

	if (size % 2 == 1) {
		// Apparement, la taille doit être paire
		unsigned char zero = 0;
		list_add_last(target_buffer_list, &zero);
		size++;
	}


	unsigned char header[8] = { 0, 0, 0, 0, 0, 0, 0, 0 };

	header[2] = size & 255;
	header[1] = (size >> 8) & 255;

	header[6] = map_40->columns - 1;
	header[7] = (map_40->lines - 1) / 8;  // Le fichier map ne fonctionne que sur multiple de 8

	fwrite(header, sizeof(unsigned char), 8, fout);

	// Ecriture du buffer map compressé dans le fichier de sortie
	for (int i = 0; i < list_size(target_buffer_list); i++) {
		list_get_at(&current, target_buffer_list, i);
		fwrite(&current, sizeof(unsigned char), 1, fout);
	}

	// Ecriture footer TO-SNAP
	unsigned char to_snap[40];

	memset(to_snap, 0, 40);
	to_snap[0] = 0; // 16 couleurs 40 colonnes
	to_snap[2] = 0; // tour de l'écran
	to_snap[4] = 0; // mode 0 console
	for (int i = 0; i < 16; i++) {
		r = palette->colors[i][0];
		g = palette->colors[i][1];
		b = palette->colors[i][2];
		short thomson_palette_value = get_palette_thomson_value(r, g, b);
		to_snap[6 + i * 2] = (thomson_palette_value >> 8) & 255;;
		to_snap[6 + i * 2 + 1] = thomson_palette_value & 255;
	}
	to_snap[38] = 0xA5;
	to_snap[39] = 0x5A;
	fwrite(to_snap, sizeof(unsigned char), 40, fout);

	// Ecriture du footer
	unsigned char footer[] = { 0, 0, 0, 0, 0 };

	footer[0] = 255;
	fwrite(footer, sizeof(unsigned char), 5, fout);


	fflush(fout);
	fclose(fout);
	list_destroy(buffer_list);
	list_destroy(target_buffer_list);

	printf("Le fichier TO-SNAP est créé\n");

	// Ecriture du chargeur TO-SNAP
	char fname_snap_out[256];

	sprintf(fname_snap_out, "%s.bld", filename);
	FILE *tosnap_out = fopen(fname_snap_out, "w");

	fprintf(tosnap_out, "10 DIM T%%(10000)\n");
	fprintf(tosnap_out, "20 DEFFNC(R)=MAX(-R-1,R)\n");
	fprintf(tosnap_out, "30 LOADP \"%s\",T%%(10000)\n", map_filename);
	fprintf(tosnap_out, "40 T=T%%(10000)\n");
	fprintf(tosnap_out, "50 T=T+1 : IF T%%(T)<>-23206 THEN END\n");
	fprintf(tosnap_out, "60 FOR I=15 TO 0 STEP -1:T=T+1:PALETTE I,FNC(T%%(T)):NEXT\n");
	fprintf(tosnap_out, "70 T=T+1 : CONSOLE,,,,T%%(T)\n");
	fprintf(tosnap_out, "80 T=T+1 : SCREEN,,T%%(T)\n");
	fprintf(tosnap_out, "90 T=T+1 : POKE &H605F,T%%(T)\n");
	fprintf(tosnap_out, "100 PUT(0,0),T%%(10000)\n");
	fflush(tosnap_out);
	fclose(tosnap_out);

	fflush(stdout);
}
