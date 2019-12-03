#ifndef HUFFHEAP_H
#define HUFFHEAP_H

/*
 * Defines a heap for use in huffman tree encoding
 */

#include "pixel.h"

typedef struct HuffHeapNode
{
	Pixel *data;
	int frequency;
	struct HuffHeapNode *left, *right;
} HuffHeapNode;

typedef struct HuffHeap
{
	int size;
	int capacity;
	HuffHeapNode **nodes;
} HuffHeap;


/**
 * Printing function for huff heap
 */
void print_huffheap(const HuffHeap *heap)
{
	for (int i = 0; i < heap->size; i++)
	{
		printf("%s : %d\n", pix_string(heap->nodes[i]->data), heap->nodes[i]->frequency);
	}
}

/**
 * Create empty heap
 *
 * Returns: pointer to new empty heap
 */
HuffHeap *create_huffheap(int initial_capacity)
{
	HuffHeap *ret = (HuffHeap *)malloc(sizeof(HuffHeap));

	ret->size = 0;
	ret->capacity = initial_capacity;
	ret->nodes = (HuffHeapNode **)malloc(sizeof(HuffHeapNode *) * ret->capacity);

	return ret;
}

/**
 * Resizes huff heap
 *
 * Returns: 1 for success, 0 for failure
 */
int resize_huffheap(HuffHeap *heap, int new_capacity)
{
	// create and copy
	HuffHeapNode **resized_list = (HuffHeapNode **)malloc(sizeof(HuffHeapNode *) * new_capacity);
	for (int i = 0; i < heap->size; i++)
	{
		resized_list[i] = heap->nodes[i];
	}

	// point to new list and free old list
	free(heap->nodes);
	heap->nodes = resized_list;
	heap->capacity = new_capacity;

	return 1;
}

/**
 * Adds node to heap or adds to frequency of existing node
 * 
 * Returns: pointer to the node
 */
HuffHeapNode *create_or_add_huffheapnode(HuffHeap *heap, const Pixel *p)
{
	// check for existing node
	for (int i = 0; i < heap->size; i++)
	{
		// If found, increment frequency of found node and return 
		if ( cmp_pix(heap->nodes[i]->data, p) )
		{
			//printf("Found existing. Frequency going from %d to %d\n", heap->nodes[i]->frequency, heap->nodes[i]->frequency + 1);
			heap->nodes[i]->frequency += 1;
			return heap->nodes[i];
		}
	}

	//resize if necessary
	if (heap->size == heap->capacity)
	{
		//printf("Resizing from %d to %d\n", heap->capacity, (int)(heap->capacity * 1.5));
		resize_huffheap(heap, (int)(heap->capacity * 1.5));
	}

	// no existing node found
	//
	// Create new node with frequency 1 and copy pixel data to it
	HuffHeapNode *ret = (HuffHeapNode *)malloc(sizeof(HuffHeapNode));
	ret->data = (Pixel *)malloc(sizeof(Pixel));
	copy_pix(ret->data, p);
	ret->frequency = 1;
	ret->left  = NULL;
	ret->right = NULL;

	heap->nodes[heap->size] = ret;
	heap->size += 1;

	return ret;
}

/**
 * Swaps two nodes in huff heap
 */
void swap_huffheapnode(HuffHeapNode **a, HuffHeapNode **b)
{
	HuffHeapNode *temp = *a;
	*a = *b;
	*b = temp;
}

/**
 * Does min heapify
 */
void min_huffheapify(HuffHeap *heap, int ii)
{
	int min   = ii;
	int left  = 2*ii + 1;
	int right = 2*ii + 2;

	// find min
	if (left < heap->size && heap->nodes[left]->frequency < heap->nodes[min]->frequency)
	{
		min = left;
	}
	if (right < heap->size && heap->nodes[right]->frequency < heap->nodes[min]->frequency)
	{
		min = right;
	}

	if (min != ii) // at least one of the above was true
	{
		swap_huffheapnode(&heap->nodes[min], &heap->nodes[ii]);
		min_huffheapify(heap, min);
	}
}

/**
 * extracts minimum node in heap
 *
 * Returns: pointer to min node
 */
HuffHeapNode *extract_min_huffheapnode(HuffHeap *heap)
{
	//printf("Extracting...\n");
	// get first node (min)
	// set first node to last node
	// heapify
	HuffHeapNode *ret = heap->nodes[0];
	heap->nodes[0] = heap->nodes[heap->size - 1];
	heap->size -= 1;
	
	//printf("Got %s\nHeapifying...\n", pix_string(ret->data));

	min_huffheapify(heap, 0);

	return ret;
}

/**
 * Inserts node into huffman min heap
 */
void insert_huffheapnode(HuffHeap *heap, HuffHeapNode *node)
{
	//printf("Inserting %s : %d\n", pix_string(node->data), node->frequency);
	int i = heap->size;
	heap->size += 1;

	while (i > 0 && node->frequency < heap->nodes[(i - 1)/2]->frequency)
	{
		heap->nodes[i] = heap->nodes[(i - 1)/2];
		i = (i - 1)/2;
	}

	heap->nodes[i] = node;
}

/**
 * Builds the min heap
 */
void build_min_huffheap(HuffHeap *heap)
{
	//location of last node
	int n = heap->size - 1;
	
	for (int i = (n - 1)/2; i >= 0; i--)
	{
		min_huffheapify(heap, i);
	}
}

/**
 * Checks whether node is a leaf node
 *
 * Returns: 1 if leaf, 0 if not leaf
 */
int is_leaf(HuffHeapNode *node)
{
	if (node->left == NULL && node->right == NULL)
		return 1;
	else return 0;
}


/**
 * Builds the huffman tree from existing Huffheap
 *
 * Returns: root HuffHeapNode of tree
 */
HuffHeapNode *build_huffmantree(HuffHeap *heap)
{
	//printf("Resizing...\n");
	if (heap->capacity > heap->size)
	{
		resize_huffheap(heap, heap->size);
	}
	//printf("Resize worked!\nPrinting...\n");
	print_huffheap(heap);	

	HuffHeapNode *top, *left, *right;
	Pixel *internal = (Pixel *)malloc(sizeof(Pixel));
	internal->r = -1;
	internal->g = -1;
	internal->b = -1;

	print_pix(internal);

	// keep going until heap has size 1
	while (heap->size > 1)
	{
		// extract min two nodes
		left  = extract_min_huffheapnode(heap);
		right = extract_min_huffheapnode(heap);

		// create internal node with frequency equal to sum of left frequency and right frequency
		// will have pixel value of (-1, -1, -1) which will be a default
		top = (HuffHeapNode *)malloc(sizeof(HuffHeapNode));
		top->data = internal;
		top->frequency = left->frequency + right->frequency;
		top->left = left;
		top->right = right;

		// insert new node into heap
		insert_huffheapnode(heap, top);
	}

	// return the min (top) of the built tree
	HuffHeapNode *ret = extract_min_huffheapnode(heap);
	//printf("Built!  Returning %s...\n", pix_string(ret->data));
	return ret;
}

/**
 * Generate Huffman Encoding
 */
void generate_huffcodes(const HuffHeapNode *root, Pixel *pix_array[], char *code_array[], char *current_code, int top, int *ind)
{
	// go left - add 0 to current code
	if (root->left != NULL)
	{
		current_code[top] = '0';
		generate_huffcodes(root->left, pix_array, code_array, current_code, top + 1, ind);
	}

	// go right - add 1 to current code
	if (root->right != NULL)
	{
		current_code[top] = '1';
		generate_huffcodes(root->right, pix_array, code_array, current_code, top + 1, ind);
	}

	if (is_leaf(root))
	{
		// add leaf to pixel array
		pix_array[*ind] = root->data;

		//printf("At ind %d got leaf %s\n", *ind, pix_string(root->data));

		// copy code (don't want to mess with working code) and add to code array
		char *copy_code = (char *)malloc( top * sizeof(char) );
		for (int i = 0; i < top; i++)
		{
			copy_code[i] = current_code[i];
		}
		code_array[*ind] = copy_code;

		// increment code and pix array counter
		*ind += 1;
	}	
}

#endif
