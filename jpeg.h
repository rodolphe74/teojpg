#ifndef JPEG_H
#define JPEG_H

//#include "log.h"



//------------------------------------------------------------------------------
// Charge une image jpeg et retourne un pointeur sur IMAGE
// A l'appelant de liberer la memoire avec free_image
//------------------------------------------------------------------------------
IMAGE *load(char *filename);

//------------------------------------------------------------------------------
// libere la memoire allouee pointee par *image
//------------------------------------------------------------------------------
void free_image(IMAGE *image);

#endif
