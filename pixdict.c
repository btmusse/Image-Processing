/*
 * Implementation of pixel dictionary utility functions
 */

#include "pixdict.h"

#include <stdlib.h>

// Find Pixel in dictionary
PD_Entry_p PD_find(const PD_p dict, const Pixel *pixel)
{
	// loop through entries
	for ( int i = 0; i < dict->size; i++ )
	{
		// compare pixels
		if ( pix_cmp(pixel, dict->entries[i]->pixel) )
		{
			// found!
			return dict->entries[i]->pixel;
		}
	}

	// not found :(
	return NULL;
}

// Create entry in dictionary
void PD_create(PD_p dict, const Pixel *pixel)
{
	// Increment size and add entry
	dict->entries[size++] = (PD_Entry_p) malloc( sizeof( PD_Entry ) );

	// Default entry values
	dict->entries[size - 1]->count = 0;
	dict->entries[size - 1]->pixel.r = pixel->r;
	dict->entries[size - 1]->pixel.g = pixel->g;
	dict->entries[size - 1]->pixel.b = pixel->b;
}

// Add pixel to already existing count
void PD_add(PD_Entry_p)
{
}

// Insert Pixel into dictionary
// Will return a code
// 0: failure
// 1: new
// 2: increase count
int PD_insert(PD_p dict, const Pixel *pixel)
{
}
