#ifndef OCTREE_H
#define OCTREE_H

#include "pixel.h"

struct OCTREE_STRUCT {
	unsigned int		count;
	unsigned long		c[3];
	struct OCTREE_STRUCT *	childs[8];
	unsigned int		level;
};
typedef struct OCTREE_STRUCT OCTREE;

struct OCTREE_CONTAINER_STRUCT {
	OCTREE *octree;
	OCTREE *parent;
	int	child_index;
};
typedef struct OCTREE_CONTAINER_STRUCT OCTREE_CONTAINER;



//------------------------------------------------------------------------------
// Trouve une palette adaptee a l'image avec octree
//------------------------------------------------------------------------------
void guess_palette_octree(IMAGE *image, PALETTE *palette, int reduc_size);


#endif
