#ifndef MAPUTIL_H
#define MAPUTIL_H

#include <string.h>

static int compare_int(const void *const one, const void *const two)
{
	const unsigned int a = *(unsigned int *)one;
	const unsigned int b = *(unsigned int *)two;

	return a - b;
}

static int compare_c_string(const void *const one, const void *const two)
{
	const unsigned char *a = (char *)one;
	const unsigned char *b = (char *)two;

	return strcmp(a, b);
}

#endif
