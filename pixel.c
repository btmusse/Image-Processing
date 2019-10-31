/*
 * Defines pixel functions
 */
#include "pixel.h"

#include <stdio.h>

void print_pix(const Pixel *pixel)
{
	printf("Pixel: (%d, %d, %d)\n", pixel->r, pixel->b, pixel->g);
}

int cmp_pix(const Pixel *a, const Pixel *b)
{
	if (a->r == b->r && a->g == b->g && a->b == b->b)
		return 1;
	else return 0;
}
