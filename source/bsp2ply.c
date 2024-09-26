/*
MIT License

Copyright (c) 2023 erysdren (it/she/they)

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

/*
 *
 * headers
 *
 */

/* std */
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <float.h>
#include <math.h>
#include <string.h>

/* glprey */
#include "bsp.h"

/*
 *
 * functions
 *
 */

/*
 * main
 */

int main(void)
{
	/* variables */
	bsp_t *bsp;
	int i, v;
	FILE *ply;

	/* read bsp */
	bsp = bsp_read("DEMO4.BSP");
	if (!bsp) return 1;

	/* write ply */
	ply = fopen("DEMO4.BSP.ply", "wb");
	if (!ply) return 1;

	/* write header */
	fprintf(ply, "ply\nformat ascii 1.0\n");
	fprintf(ply, "element vertex %d\n", bsp->num_vertices);
	fprintf(ply, "property float x\nproperty float y\nproperty float z\n");
	fprintf(ply, "element face %d\n", bsp->num_polygons);
	fprintf(ply, "property list uchar uint vertex_indices\n");
	fprintf(ply, "end_header\n");

	/* write vertices */
	for (i = 0; i < bsp->num_vertices; i++)
	{
		fprintf(ply, "%0.6f %0.6f %0.6f\n",
			bsp->xcomponents[bsp->vertices[i].x],
			bsp->ycomponents[bsp->vertices[i].y],
			bsp->zcomponents[bsp->vertices[i].z]
		);
	}

	/* write polygons */
	for (i = 0; i < bsp->num_polygons; i++)
	{
		fprintf(ply, "%d", bsp->polygons[i].num_verts);

		for (v = 0; v < bsp->polygons[i].num_verts; v++)
		{
			fprintf(ply, " %d", bsp->polygons[i].verts[v]);
		}

		fprintf(ply, "\n");
	}

	/* free memory */
	fclose(ply);
	bsp_free(bsp);

	/* return success */
	return 0;
}
