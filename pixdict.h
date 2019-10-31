#ifndef PIXDICT_H
#define PIXDICT_H

/*
 * Defines a pixel dictionary for huffman coding
 */

#include "pixel.h"

typedef struct PD_Entry
{
	const Pixel *pixel;
	int count;
} PD_Entry, *PD_Entry_p

typedef struct PD
{
	PD_Entry_p *entries;
} PD, *PD_p

// Find Pixel in dictionary
PD_Entry_p PD_find(const PD_p dict, const Pixel *pixel);

// Insert Pixel into dictionary
// Will return a code
// 0: failure
// 1: new
// 2: increase count
int PD_insert(PD_p dict, const Pixel *pixel);

// Create entry in dictionary
// Will return a boolean success code
int PD_create(PD_p dict, const Pixel *pixel);

// Add pixel to already existing count
// Will return a boolean success code
int PD_add(PD_Entry_p);

#endif
