#ifndef KMEAN_H
#define KMEAN_H

#include "pixel.h"
#include <vector.h>

struct COLOR_STRUCT {
    unsigned char r;
    unsigned char g;
    unsigned char b;
};
typedef struct COLOR_STRUCT COLOR;



struct CLUSTER_LIST_STRUCT {
    vector cluster_vector;
};
typedef struct CLUSTER_LIST_STRUCT CLUSTER_LIST;



//------------------------------------------------------------------------------
// Trouve une palette adaptee a l'image avec kmean
//------------------------------------------------------------------------------
void guess_palette_kmean(IMAGE *image, PALETTE *palette, int reduc_size);


#endif