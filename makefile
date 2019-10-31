test: huffman_test.c
	gcc -o test pixel.c huffman_test.c

clear:
	rm -f test
