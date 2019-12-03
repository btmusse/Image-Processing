/*
 * This is a test of Huffman Coding
 *
 * Created 10/28/19
 */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

// OpenMP
#include <omp.h>

// Pixel Utils
#include "pixel.h"
#include "huffheap.h"

int main(int argc, char *argv[])
{
	int num_proc;

	int numrows = 4, numcols = 4;

	int j;
	Pixel **test_image = (Pixel *)malloc( numrows * sizeof(Pixel) );
	HuffHeap *heap = create_huffheap(6);
	

#pragma omp parallel for private(j)
	for( int i = 0; i < numrows; i++ )
	{
		test_image[i] = (Pixel *)malloc( numcols * sizeof(Pixel) );
		//printf("Row %d:\n", i);
		for (j = 0; j < numcols; j++)
		{
			//create
			test_image[i][j].r = rand() % 2;
			test_image[i][j].g = rand() % 2;
			test_image[i][j].b = rand() % 2;
			printf("%d is adding ", omp_get_thread_num());
			print_pix(&test_image[i][j]);
			//add to hash
#pragma omp critical
			create_or_add_huffheapnode(heap, &test_image[i][j]);
		}
	}

	//printing
	int heapsize = heap->size;
	print_huffheap(heap);
	
	build_min_huffheap(heap);
	//print_huffheap(heap);
	HuffHeapNode *treeroot = build_huffmantree(heap);
	//print_huffheap(heap);
	Pixel *pix_list[heapsize];
	char *code_list[heapsize];
	char *currentcode = (char *)malloc(sizeof(char) * 200);
	int *ind = (int *)malloc(sizeof(int));
	*ind = 0;
	generate_huffcodes(treeroot, pix_list, code_list, currentcode, 0, ind);
	
	for (int i = 0; i < heapsize; i++)
	{
		printf("%s : %s\n", pix_string(pix_list[i]), code_list[i]);
	}

	//free
	free(test_image);
	//free(pix_row);
	return 0;
}
