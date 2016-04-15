#define _USE_MATH_DEFINES

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "Source.h"

#define IOBUFFSIZE 1024

int main(void) {

	int D = 2; //downsample by 2
	int U = 1;
	
	FILE *fx = fopen("freq94_8_bw_4.bin", "rb"); //open input file
	FILE *fh1 = fopen("lpf_U1_D2_L100_80_1.bin", "rb"); //open inpulse file
	//FILE *fh2 = fopen("lpf_U1_D2_L100_40_2.bin", "rb"); //open inpulse file
	//FILE *fh3 = fopen("lpf_U1_D2_L100_20_3.bin", "rb"); //open inpulse file
	//FILE *fh4 = fopen("lpf_U1_D2_L100_10_4.bin", "rb"); //open inpulse file
	FILE *fy1 = fopen("stage1.bin", "wb"); //open output file
	//FILE *fy2 = fopen("stage2.bin", "wb"); //open output file
	//FILE *fy3 = fopen("stage3.bin", "wb"); //open output file
	//FILE *fy4 = fopen("stage4.bin", "wb"); //open output file

	dsp_file_header headh;
	fread(&headh, sizeof(dsp_file_header), 1, fh1);
	int M = headh.d0;

	float *x = (float*)calloc(sizeof(float), IOBUFFSIZE * 2);
	float *h = (float*)calloc(sizeof(float), headh.d0);
	float *y = (float*)calloc(sizeof(float), IOBUFFSIZE * 2);
	float *rg = (float*)calloc(sizeof(float), IOBUFFSIZE * 2);
	float *ig = (float*)calloc(sizeof(float), IOBUFFSIZE * 2);
	float rx[IOBUFFSIZE], ix[IOBUFFSIZE];// , ry[IOBUFFSIZE], iy[IOBUFFSIZE];

	//garbage
	fread(x, sizeof(float), 100 * 2, fx);	

	fread(h, sizeof(float), headh.d0, fh1);//read in the impulse response in natural order

	//processing
	float rt,it;//variable for acccumulating convolution result
	int xlen, ylen = 0;//indexes for input and out put buffers
	int i;//index for input data buffers
	int j;
	int k = 0;//indexforcirculardatabuffer
	int l = 1;
	int m;
	int n;//convolution loop index for filter coefficients and circular data buffer
	float f = -0.0375;
	xlen = fread(x, sizeof(float), IOBUFFSIZE*2, fx);//read in first chunk of input samples
												   //split imaginary and complex
	for (i = 0; i < IOBUFFSIZE; i++) {
		rx[i] = x[i * 2] * cos(2 * M_PI*f*i)* x[i * 2 + 1] * sin(2 * M_PI*f*i);
		ix[i] = x[i * 2 + 1] * cos(2 * M_PI*f*i) - x[i * 2] * sin(2 * M_PI*f*i);
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
							fwrite(y, sizeof(float), ylen, fy1);
							ylen = 0;
						}
					}
				}
			}
			xlen = fread(x, sizeof(float), IOBUFFSIZE, fx);
			for (i = 0; i < IOBUFFSIZE; i++) {
				rx[i] = x[i * 2] * cos(2 * M_PI*f*i)* x[i * 2 + 1] * sin(2 * M_PI*f*i);
				ix[i] = x[i * 2 + 1] * cos(2 * M_PI*f*i) - x[i * 2] * sin(2 * M_PI*f*i);
			}
		}
	//Finish writing last chunk of output samples
	if (ylen>0) {
		fwrite(y, sizeof(float), ylen, fy1);
		ylen = 0;
	}

	fclose(fx);
	fclose(fy1);
	fclose(fh1);
	free(x);
	free(h);
	free(y);

	return 1;
}

