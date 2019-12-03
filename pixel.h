#ifndef PIXEL_H
#define PIXEL_H

/*
 * Defines a pixel type
 */

#include <stdio.h>

typedef struct Pixel
{
	short r;
	short b;
	short g;
} Pixel;

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

void copy_pix(Pixel *to, const Pixel *from)
{
	to->r = from->r;
	to->g = from->g;
	to->b = from->b;
}

char *pix_string(const Pixel *pixel)
{
	char *ret = (char *)malloc(15 * sizeof(char)); // format: (000, 000, 000)
	sprintf(ret, "(%03d, %03d, %03d)", pixel->r, pixel->b, pixel->g);

	return ret;
}

#endif
