#include "octree.h"
#include <list.h>
#include <map.h>
#include <math.h>
#include <string.h>


void create_key(OCTREE *octree, char *key)
{
	char ref[64];

	sprintf(key, "%05d", octree->count);
	sprintf(ref, "%p", (void *)octree);
	strcat(key, "_");
	strcat(key, ref);
}

int get_bit(unsigned char byte, int idx)
{
	return (byte & (int)pow(2, idx)) >> idx;
}

int get_idx(int r, int g, int b, int idx)
{
	int r_bit = get_bit(r, idx);
	int g_bit = get_bit(g, idx);
	int b_bit = get_bit(b, idx);

	return r_bit << 2 | g_bit << 1 | b_bit;
}

void init_childs(OCTREE *octree)
{
	for (int i = 0; i < 8; i++)
		octree->childs[i] = NULL;
	octree->count = 0;
	octree->c[0] = 0;
	octree->c[1] = 0;
	octree->c[2] = 0;
}

int is_leaf(OCTREE *octree)
{
	int leaf = 1;

	for (int i = 0; i < 8; i++) {
		if (octree->childs[i] != NULL) {
			leaf = 0;
			break;
		}
	}
	return leaf;
}

void display_octree(OCTREE *octree)
{
	if (is_leaf(octree)) {
		printf("%p *%d (%d,%d,%d) (%d,%d,%d)\n", (void *)octree, octree->count,
		       octree->c[0], octree->c[1], octree->c[2],
		       octree->c[0] / octree->count, octree->c[1] / octree->count, octree->c[2] / octree->count);
	} else {
		for (int i = 0; i < 8; i++) {
			if (octree->childs[i] != NULL)
				// printf("  %p *%d\n", (void*) octree, i);
				display_octree(octree->childs[i]);
		}
	}
}

void free_octree(OCTREE *octree)
{
	if (is_leaf(octree)) {
		// printf("free %p\n", (void*) octree);
		free(octree);
	} else {
		for (int i = 0; i < 8; i++) {
			if (octree->childs[i] != NULL) {
				// printf("  %p *%d\n", (void*) octree, i);
				free_octree(octree->childs[i]);
				octree->childs[i] = NULL;
			}
		}
	}
}


void count_leaves(OCTREE *octree, int *count)
{
	if (is_leaf(octree)) {
		(*count)++;
	} else {
		for (int i = 0; i < 8; i++)
			if (octree->childs[i] != NULL)
				count_leaves(octree->childs[i], count);
	}
}


void convert_octree_to_palette(OCTREE *octree, PALETTE *p, int *index)
{
	if (is_leaf(octree)) {
		p->colors[*index][0] = (int)(octree->c[0] / octree->count);
		p->colors[*index][1] = (int)(octree->c[1] / octree->count);
		p->colors[*index][2] = (int)(octree->c[2] / octree->count);
		(*index)++;
	} else {
		for (int i = 0; i < 8; i++) {
			if (octree->childs[i] != NULL)
				// printf("  %p *%d\n", (void*) octree, i);
				convert_octree_to_palette(octree->childs[i], p, index);
		}
	}
}


void display_octree_list(list *leaves)
{
	char key[128];
	OCTREE_CONTAINER current;

	for (int i = 0; i < list_size(*leaves); i++) {
		list_get_at(&current, *leaves, i);
		create_key(current.octree, key);
		printf("> %s   %p %d [%p] = %d\n", key, (void *)current.octree, current.octree->level, current.parent, current.octree->count);
	}
}


void create_leaf_list(OCTREE *octree, int child_index, OCTREE *parent, int *depth, list *reducible_leafs)
{
	if (is_leaf(octree)) {
		/*
		 * printf("%d - %p[%d] = %p | (%d %d %d) %d \n", *depth, (void *) parent, child_index, (void *) octree,
		 *      octree->c[0]/octree->count, octree->c[1]/octree->count, octree->c[2]/octree->count, octree->count);
		 */

		octree->level = *depth;

		OCTREE_CONTAINER octree_container;
		octree_container.octree = octree;
		octree_container.parent = parent;
		// printf("affectation parent %p   %p\n", parent, parent->childs[child_index]);
		octree_container.child_index = child_index;

		list_add_first(*reducible_leafs, &octree_container);
		// list_add_last(*reducible_leafs, &octree_container);
	} else {
		for (int i = 0; i < 8; i++)
			if (octree->childs[i] != NULL) {
				(*depth)++;
				create_leaf_list(octree->childs[i], i, octree, depth, reducible_leafs);
				(*depth)--;
			}
	}
}


int reduce_octree(list *reducible_leafs, int level)
{
	// printf("reduce level %d\n", level);
	OCTREE_CONTAINER current;
	int decrease_count = -1;        // -1, pas trouvé, 0 ou 1 = nombre de feuilles en moins

	for (int i = 0; i < list_size(*reducible_leafs); i++) {
		list_get_at(&current, *reducible_leafs, i);
		// printf("> %p %d [%p]\n", (void *) current.octree, current.octree->level, current.parent);

		if (current.octree->level = level) {
			decrease_count = 1;
			// printf("> %p %d [%p][%d]\n", (void *) current.octree, current.octree->level, (void *) current.parent, current.child_index);

			current.parent->count += current.octree->count;
			current.parent->c[0] += current.octree->c[0];
			current.parent->c[1] += current.octree->c[1];
			current.parent->c[2] += current.octree->c[2];

			// printf("compare %p %p[%d]=%p\n", current.octree, current.parent, current.child_index, current.parent->childs[current.child_index]);

			free(current.parent->childs[current.child_index]);
			current.parent->childs[current.child_index] = NULL;

			if (is_leaf(current.parent)) {
				// Une feuille en moins mais le parent devient feuille
				// donc pas de décrementation
				decrease_count = 0;
			} else {
				decrease_count = 1;
			}

			// printf("->compare %p %p[%d]=%p\n", current.octree, current.parent, current.child_index, current.parent->childs[current.child_index]);
			list_remove_at(*reducible_leafs, i);

			break;
		}
	}
	return decrease_count;
}







void build_octree(IMAGE *image, OCTREE *octree)
{
	unsigned int r, g, b;
	int count = 0;

	for (int y = 0; y < image->height; y++) {
		for (int x = 0; x < image->width; x++) {
			r = image->pixels[x + y * image->width].r;
			g = image->pixels[x + y * image->width].g;
			b = image->pixels[x + y * image->width].b;

			// printf("rgb : %d %d %d \n", r, g, b);
			OCTREE *step = octree;

			// printf("begin variable step is at address: %p\n", (void*)step);
			// printf("begin variable octree is at address: %p\n", (void*)octree);
			for (int i = 7; i >= 0; i--) {
				int idx = get_idx(r, g, b, i);
				// printf("idx %d\n", idx);

				if (step->childs[idx] == NULL) {
					// printf("pas trouve\n");
					OCTREE *new_octree = (OCTREE *)malloc(sizeof(OCTREE));
					// printf("variable new_octree is at address: %p\n", (void*)new_octree);
					init_childs(new_octree);
					step->childs[idx] = new_octree;
					step = new_octree;
					step->count = 0;
					// printf("variable step is at address: %p\n", (void*)step);
				} else {
					// printf("trouve \n");
					step = step->childs[idx];
					// printf("variable step is at address: %p\n", (void*)step);
				}
			}
			// printf("count variable step is at address: %p\n", (void*)step);
			step->count++;
			step->c[0] += r;
			step->c[1] += g;
			step->c[2] += b;
			count++;
		}
	}
	// int leaves = 0;
	// count_leaves(octree, &leaves);
	// printf("leaves %d\n", leaves);
}



void guess_palette_octree(IMAGE *image, PALETTE *palette, int reduc_size)
{
	OCTREE *octree = malloc(sizeof(OCTREE));

	init_childs(octree);
	build_octree(image, octree);
	int leaves = 0, count;

	count_leaves(octree, &count);

	int depth = 0;
	list reducibles = list_init(sizeof(OCTREE_CONTAINER));

	int level = 8;
	int leaves_left;
	int leaves_count = count;
	int cont = 1;

	for (int l = 8; l > 0; l--) {
		depth = 0;
		list_clear(reducibles);
		create_leaf_list(octree, 0, NULL, &depth, &reducibles);

		leaves_left = 1;
		while (leaves_left > -1) {
			leaves_left = reduce_octree(&reducibles, l);
			leaves_count -= (leaves_left >= 0 ? leaves_left : 0);
			if (leaves_count <= reduc_size) {
				cont = 0;
				break;
			}
		}
		if (!cont)
			break;
	}
	int palette_index = 0;

	convert_octree_to_palette(octree, palette, &palette_index);
	palette->size = reduc_size;




	/*
	 * palette->colors[0][0] = 255;
	 * palette->colors[0][1] = 128;
	 * palette->colors[0][2] = 0;
	 */

	/*
	 * palette->colors[2][0] = 255;
	 * palette->colors[2][1] = 127;
	 * palette->colors[2][2] = 0;
	 */

	// for (int i = 0; i < 16; i += 2) {
	// 	palette->colors[i][0] = 0;
	// 	palette->colors[i][1] = 0;
	// 	palette->colors[i][2] = 0;
	// }

	// for (int i = 1; i < 16; i += 2) {
	// 	palette->colors[i][0] = 255;
	// 	palette->colors[i][1] = 255;
	// 	palette->colors[i][2] = 255;
	// }

	/*
	 * for (int i = 0; i < 16; i+=2) {
	 *      printf("%d\n", i);
	 *      RGB_COLOR c, t;
	 *      c[0] = palette->colors[i][0];
	 *      c[1] = palette->colors[i][1];
	 *      c[2] = palette->colors[i][2];
	 *      complementary_color(c, &t);
	 *      palette->colors[i + 1][0] = t[0];
	 *      palette->colors[i + 1][1] = t[1];
	 *      palette->colors[i + 1][2] = t[2];
	 * }
	 */



	list_destroy(reducibles);
	free_octree(octree);
}
