#ifndef PIXEL_H
#define PIXEL_H

/*
 * Defines a pixel type
 */

typedef struct Pixel
{
	short r;
	short b;
	short g;
} Pixel;

void print_pix(const Pixel *pixel);

int cmp_pix(const Pixel *a, const Pixel *b);

#endif
