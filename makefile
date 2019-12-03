test: huffman_test.c
	gcc -fopenmp -o test huffman_test.c

clean:
	rm -f test
