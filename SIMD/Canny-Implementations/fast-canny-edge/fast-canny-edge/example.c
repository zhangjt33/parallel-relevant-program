/*
	FAST-EDGE
	Copyright (c) 2009 Benjamin C. Haynor

	Permission is hereby granted, free of charge, to any person
	obtaining a copy of this software and associated documentation
	files (the "Software"), to deal in the Software without
	restriction, including without limitation the rights to use,
	copy, modify, merge, publish, distribute, sublicense, and/or sell
	copies of the Software, and to permit persons to whom the
	Software is furnished to do so, subject to the following
	conditions:

	The above copyright notice and this permission notice shall be
	included in all copies or substantial portions of the Software.

	THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
	EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
	OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
	NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
	HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
	WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
	FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
	OTHER DEALINGS IN THE SOFTWARE.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include "imageio.h"
#include "fast-edge.h"

int main(int argc, char *argv[])
{
	FILE *fp;
	int w, h, i;
	if ((fp = fopen(argv[1], "r")) == NULL && argc == 1) {
		printf("ERROR: %s can't open %s!", argv[0], argv[1]);
	} else {
		if (read_pgm_hdr(fp, &w, &h) != -1) {
			struct image img, img_gauss, img_out; //img_scratch, img_scratch2, 
			printf("*** PGM file recognized, reading data into image struct ***\n");
			img.width = w;
			img.height = h;
			unsigned char *img_data = malloc(w * h * sizeof(char));
			for (i = 0; i < w * h; i++) {
				img_data[i] = fgetc(fp);
			}
			img.pixel_data = img_data;
			img_out.width = img_gauss.width = w;
			img_out.height = img_gauss.height = h;
			unsigned char *img_gauss_data = malloc(w * h * sizeof(char));
			img_gauss.pixel_data = img_gauss_data;
			unsigned char *img_out_data = malloc(w * h * sizeof(char));
			img_out.pixel_data = img_out_data;
			printf("*** image struct initialized ***\n");
			printf("*** performing gaussian noise reduction ***\n");
			gaussian_noise_reduce(&img, &img_gauss);
			//printf("*** performing morphological closing ***\n");
			//morph_close(&img, &img_scratch, &img_scratch2, &img_gauss);
			canny_edge_detect(&img_gauss, &img_out);
			write_pgm_image(&img_out);
			free(img_data);
			free(img_gauss_data);
			free(img_out_data);
		} else {
			printf("ERROR: %s is not a PGM file!", argv[1]);
		}
	}
	return(1);
}
