#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "Source.h"

#define IOBUFFSIZE 1024

int main(void) {

//	float M = pow(10, 6);
//	float Ft = 94.5 * M;
//	float Fc = 98.4 * M;
//	float Fs = 8 * M;
	int D = 2; //downsample by 2
	int U = 1;
	
	/*
	//Get input length
	int Lx = 0, countl;
	FILE *fxl = fopen("freq94_8_bw_4.bin", "rb"); //open file
	float *xl = (float*)calloc(sizeof(float), IOBUFFSIZE); //create temp in buffer
	fread(xl, sizeof(float), 100 * 2, fxl); //remove garbage
	countl = fread(xl, sizeof(float), IOBUFFSIZE, fxl); //get first count of samples
	while (countl == IOBUFFSIZE) { //check for less than 1024
		Lx += IOBUFFSIZE; //add 1024
		fread(xl, sizeof(float), IOBUFFSIZE, fxl); //read another set
	}
	Lx += countl; //add remainder
	fclose(fxl); //close file
	free(xl);  //free xl
	*/

	FILE *fx = fopen("freq94_8_bw_4.bin", "rb"); //open input file
	FILE *fh = fopen("lpf_U1_D2_L100.bin", "rb"); //open inpulse file
	FILE *fy = fopen("outfile.bin", "wb"); //open output file

	dsp_file_header headh, heady;
	fread(&headh, sizeof(dsp_file_header), 1, fh);
	/*heady.ndim = 1;
	heady.nchan = 2;
	heady.d0 = Lx*headh.d0/D-1;
	heady.d1 = 0;
	heady.d2 = 0;
	fwrite(&heady, sizeof(int), 5, fy);*/

	int M = headh.d0;


	float *x = (float*)calloc(sizeof(float), IOBUFFSIZE * 2);
	float *h = (float*)calloc(sizeof(float), headh.d0);
	float *y = (float*)calloc(sizeof(float), IOBUFFSIZE * 2);
	float *rg = (float*)calloc(sizeof(float), IOBUFFSIZE * 2);
	float *ig = (float*)calloc(sizeof(float), IOBUFFSIZE * 2);
	float rx[IOBUFFSIZE], ix[IOBUFFSIZE], ry[IOBUFFSIZE], iy[IOBUFFSIZE];

	//garbage
	fread(x, sizeof(float), 100 * 2, fx);

	int totCount=0, count;

	//initial read
	//count = fread(x, sizeof(float), IOBUFFSIZE * 2, fx);
	//totCount += count;

	

	fread(h, sizeof(float), headh.d0, fh);//read in the impulse response in natural order
	//processing
	float rt,it;//variable for acccumulating convolution result
	int xlen, ylen = 0;//indexes for input and out put buffers
	int i;//index for input data buffers
	int j;
	int k = 0;//indexforcirculardatabuffer
	int l = 1;
	int m;
	int n;//convolution loop index for filter coefficients and circular data buffer
	xlen = fread(x, sizeof(float), IOBUFFSIZE*2, fx);//read in first chunk of input samples
												   //split imaginary and complex
	for (int i = 0; i < IOBUFFSIZE; i++) {
		rx[i] = x[i * 2];
		ix[i] = x[i * 2 + 1];
	}
		while (xlen>0) {
			for (i = 0; i<xlen; i++) {
				k = (k + M - 1) % M;
				rg[k] = rx[i];
				ig[k] = ix[i];
				l = (l + D - 1) % D;
				if (l == 0) {
					l = D;
					for (j = 0; j<U; j++) {
						for (rt = 0.0, it = 0.0, m = 0, n = 0; n<M; n++, m += U) {
							rt += h[m + j] * rg[(n + k) % M];
							it += h[m + j] * ig[(n + k) % M];
						}
						y[ylen*2] = rt;
						y[ylen * 2 + 1] = it; 
						ylen++;
						if (ylen == IOBUFFSIZE) {
							fwrite(y, sizeof(float), ylen, fy);
							ylen = 0;
						}
					}
				}
			}
			xlen = fread(x, sizeof(float), IOBUFFSIZE, fx);
		}
	//Finish writing last chunk of output samples
	if (ylen>0) {
		fwrite(y, sizeof(float), ylen, fy);
		ylen = 0;
	}

	fclose(fx);
	fclose(fy);
	fclose(fh);
	free(x);
	free(h);
	free(y);

	return 1;
}

