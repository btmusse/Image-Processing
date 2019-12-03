#include<stdio.h>
#include<stdlib.h>
#include<time.h>
#include "lzss.h"

int main(int argc, char *argv[]){
	FILE *in;
	FILE *out;
	double elapsed, start;


	in=fopen("ppm_images/4x4.ppm", "r");
	//in=fopen("ppm_images/snail.ascii.ppm", "r");
	//in=fopen("ppm_images/star_field.ascii.ppm", "r");
	out =fopen("compression.out", "w");
	start = clock();
	EncodeLZSS(in, out, argc, argv);
	elapsed = ((double) clock() - start) / CLOCKS_PER_SEC;
	printf("%f \n", elapsed);
	fclose(in);
	fclose(out);
	return 0;
}
