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

void write_pgm_image(struct image * img) 
{
	FILE *fp_out;
	int i = 0;
	if((fp_out =fopen("fast_canny_output.pgm","w"))== 0)
		printf("Error opening output file.");
	fprintf(fp_out, "P5\n#FAST-EDGE\n%d %d\n255\n", img->width, img->height);
	for(i = 0; i < (img->height * img->width); i++)
		fputc(img->pixel_data[i], fp_out);
	fclose(fp_out);
}

int read_pgm_hdr(FILE *fp, int *w, int *h)
{
	char filetype[4];
	int maxval;
	if(skipcomment(fp) == EOF || fscanf(fp, "%2s\n", filetype) != 1 || strcmp(filetype, "P5") || skipcomment(fp) == EOF || fscanf(fp, "%d", w) != 1 || skipcomment(fp) == EOF || fscanf(fp, "%d", h) != 1 || skipcomment(fp) == EOF || fscanf(fp, "%d%*c", &maxval) != 1 || maxval > 255) {
		return(-1);
	} else { 
		return(0);
	}
}

int skipcomment(FILE *fp)
{
	int i;
	if((i = getc(fp)) == '#') {
		while((i = getc(fp)) != '\n' && i != EOF);
	}
	return(ungetc(i, fp));
}
