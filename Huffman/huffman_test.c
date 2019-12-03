/*
 * This is a test of Huffman Coding
 *
 * Created 10/28/19
 */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <time.h>

// OpenMP
#include <omp.h>

// Pixel Utils
#include "pixel.h"
#include "huffheap.h"

int main(int argc, char *argv[])
{
	// open file
	char buff[1000];
	FILE *file;
	if ((file = fopen(argv[2], "r")) == NULL)
	{
		printf("Error opening %s\n", argv[2]);
		exit(1);
	}

	// dummy name
	fscanf(file, "%[^\n]", buff);
	//printf("%s\n", buff);

	int numrows, numcols;
	fscanf(file, "%s", buff);
	numrows = atoi(buff);
	fscanf(file, "%s", buff);
	numcols = atoi(buff);

	//printf("%d by %d\n", numrows, numcols);

	fscanf(file, "%s", buff);
	if (atoi(buff) > 255)
	{
		printf("Max > 255\n");
		exit(1);
	}

	Pixel *image[numrows]; // this took me two hours to solve :)
	//read image
	for (int i = 0; i < numrows; i++)
	{
		image[i] = (Pixel *)malloc( numcols * sizeof(Pixel) );
		for (int j = 0; j < numcols; j++)
		{
			fscanf(file, "%hd %hd %hd", &image[i][j].r, &image[i][j].g, &image[i][j].b);
//			print_pix(&image[i][j]);
		}
//		printf("\n");
	}

	/* STEP 1: CREATE HUFFMAN TREE */
	// This part does not parallelize well because there are too many critical sections when building the minheap and Huffman Tree

	HuffHeap *heap = create_huffheap(numrows * numcols);

	int j;
	double elapsed = omp_get_wtime();
	//init omp
omp_set_num_threads(atoi(argv[1]));
/*
#pragma omp parallel for private(j)
	for( int i = 0; i < numrows; i++ )
	{
		image[i] = (Pixel *)malloc( numcols * sizeof(Pixel) );
		//printf("Row %d:\n", i);
		for (j = 0; j < numcols; j++)
		{
			//printf("Col %d performed by thread %d\n", j, omp_get_thread_num());
			//create
			image[i][j].r = rand() % 2;
			image[i][j].g = rand() % 2;
			image[i][j].b = rand() % 2;
			//printf("%d is adding ", omp_get_thread_num());
			//print_pix(&image[i][j]);
			//add to hash
#pragma omp critical (cr_add)
			create_or_add_huffheapnode(heap, &image[i][j]);
			
			//printf("size after cr_add: %d, thread: %d\n", heap->size, omp_get_thread_num());

//			printf("Thread %d completed j %d\n", omp_get_thread_num(), j);
		
		}
		//printf("size at end of row: %d\n", heap->size);
	}
*/

//#pragma omp parallel for private(j)
	for (int i = 0; i < numrows; i++)
	{
		for (j = 0; j < numcols; j++)
		{
//#pragma omp critical (cr_add)
			create_or_add_huffheapnode(heap, &image[i][j]);
		}
	}

	//printf("All adding done\n");

	double lap = omp_get_wtime() - elapsed;

// more shared variables	
	int heapsize = heap->size;
	Pixel *pix_list[heapsize];
	char *code_list[heapsize];
	char *currentcode = (char *)malloc(sizeof(char) * 200);
	int *ind = (int *)malloc(sizeof(int));
	*ind = 0;
	
	//printing
	//print_huffheap(heap);
	//printf("heapsize: %d\n", heapsize);

	build_min_huffheap(heap);
	
	//print_huffheap(heap);
	
	HuffHeapNode *treeroot = build_huffmantree(heap);
	
	//print_huffheap(heap);

	generate_huffcodes(treeroot, pix_list, code_list, currentcode, 0, ind);
	double laptwo = omp_get_wtime() - elapsed;

	/*
	for (int i = 0; i < heapsize; i++)
	{
		//printf("%s : %s\n", pix_string(pix_list[i]), code_list[i]);
	}
	*/


	/* STEP 2: WRITE TO OUTPUT */
	//This section done in parallel -> the brunt of the speedup

#pragma omp parallel
{
char *b = malloc(numrows * numcols * 64);
int count = 0, index = 0;
FILE *out;

#pragma omp for private(j) ordered
	for (int i = 0; i < numrows; i++)
	{
		for (j = 0; j < numcols; j++)
		{
			index = find_in_pixlist(pix_list, &image[i][j], heapsize);
			//printf("%d: Index for %s at (%d, %d) = %d\n", omp_get_thread_num(), pix_string(&image[i][j]), i, j, index);
			sprintf(&b[count], "%s", code_list[index]);
			count += strlen(code_list[index]);
			//printf("%d: %s\n", omp_get_thread_num(), code_list[index]);
			//printf("%d: count = %d\n", omp_get_thread_num(), count);
		}
		//printf("%d: %s\n", omp_get_thread_num(), b);
	}
	//printf("%s\n", b);

#pragma omp for ordered
	for(int i = 0; i < omp_get_num_threads(); i++)
	{
#pragma omp ordered
		{
			//printf("%d: %s\n", omp_get_thread_num(), b);
			if ((out = fopen(argv[3], "a+")) == NULL)
			{
				printf("Error opening %s\n", argv[3]);
				exit(1);
			}
			fprintf(out, "%s", b);
			fclose(out);
			
		}
	}
}
	elapsed = omp_get_wtime() - elapsed;
	printf("Threads: %d, Lap: %f, Lap 2: %f, Elapsed %f\n", atoi(argv[1]), lap, laptwo, elapsed);
	
	//free
	//free(image);
	//free(pix_list);
	//free(code_list);
	free(heap);
	fclose(file);
	return 0;
}
