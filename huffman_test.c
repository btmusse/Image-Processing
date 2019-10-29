/*
 * This is a test of Huffman Coding
 *
 * Created 10/28/19
 */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

// MPI
// #include <mpi.h>

// Hash search
#include <search.h>
#include <errno.h>

typedef struct Pixel
{
	short r;
	short b;
	short g;
	short a;
} Pixel;

void print_pix(const Pixel *pixel);

int cmp_pix(const Pixel *a, const Pixel *b);

int main(int argc, char *argv[])
{
	//int num_proc, rank;

	/* MPI Initialization */
	/*
	MPI_Init(&argc, &argv);
	MPI_Comm comm = MPI_COMM_WORLD;
	MPI_Comm_size(comm, &num_proc);
	MPI_Comm_rank(comm, &rank);
	*/

	int pix_row_size = 10;
	Pixel *pix_row = (Pixel *)malloc( pix_row_size * sizeof(Pixel) );

	for (int i = 0; i < pix_row_size; i++)
	{
		pix_row[i].r = i*2;
		pix_row[i].g = i*2;
		pix_row[i].b = i*2;
		pix_row[i].a = i*2;
	}

	for (int i = 0; i < pix_row_size; i++)
	{
		print_pix(&pix_row[i]);
	}

	Pixel a, b;
	a.r = 1;
	a.g = 1;
	a.b = 1;
	a.a = 1;
	b.r = 1;
	b.g = 1;
	b.b = 1;
	b.a = 1;

	if( cmp_pix(&a, &b) ) printf("Same!\n");
	else printf("Diff!\n");


	int numrows = 4, numcols = 4;
	
	/* Search Test */
	if ( hcreate(numrows * numcols) == 0 )
	{
		printf("Error creating hash of size %d\n", numrows*numcols);
		return errno;
	}
	
	Pixel **test_image = (Pixel *)malloc( numrows * sizeof(Pixel) );
	for( int i = 0; i < numrows; i++ )
	{
		test_image[i] = (Pixel *)malloc( numcols * sizeof(Pixel) );
		printf("Row %d:\n", i);
		for (int j = 0; j < numcols; j++)
		{
			//create
			test_image[i][j].r = rand() % 2;
			test_image[i][j].g = rand() % 2;
			test_image[i][j].b = rand() % 2;
			test_image[i][j].a = rand() % 2;
			print_pix(&test_image[i][j]);

			//add to hash
			//TODO
		}
	}


	//free
	hdestroy();
	free(test_image);
	free(pix_row);
	return 0;
}

void print_pix(const Pixel *pixel)
{
	printf("Pixel: (%d, %d, %d, %d)\n", pixel->r, pixel->b, pixel->g, pixel->a);
}

int cmp_pix(const Pixel *a, const Pixel *b)
{
	if (a->r == b->r && a->g == b->g && a->b == b->b && a->a == b->a)
		return 1;
	else return 0;
}
