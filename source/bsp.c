/*
MIT License

Copyright (c) 2023-2024 erysdren (it/she/they)

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
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdint.h>

/* backend */
#include "backend.h"

/* bsp */
#include "bsp.h"

/*
 *
 * functions
 *
 */

/*
 * token_read
 */

int token_read(FILE *stream, token_t *token)
{
	/* variables */
	int i;
	char c;

	/* clear string */
	for (i = 0; i < TOKEN_STR_LEN; i++) token->str[i] = '\0';

	/* parse loop */
	i = 0;
	while (!feof(stream) && i < TOKEN_STR_LEN)
	{
		/* get char */
		c = fgetc(stream);

		/* return success if we've reached a space */
		/* otherwise add to string */
		if (isspace(c))
		{
			/* if we've just started, skip */
			if (i == 0) continue;

			/* return success */
			token->len = i + 1;
			return 1;
		}
		else
		{
			token->str[i++] = c;
		}
	}

	/* return failure */
	return 0;
}

/*
 * token_string
 */

int token_string(token_t *token, const char *string)
{
	if (strcmp(token->str, string) != 0)
		return 0;

	return 1;
}

/*
 * token_expect
 */

int token_expect(FILE *stream, token_t *token, const char *string)
{
	token_read(stream, token);
	return token_string(token, string);
}

/*
 * print_vec3
 */

void print_vec3(FILE *stream, vec3_t *vec)
{
	fprintf(stream, "%0.6f %0.6f %0.6f", vec->x, vec->y, vec->z);
}

/*
 * print_vec3i
 */

void print_vec3i(FILE *stream, vec3i_t *vec)
{
	fprintf(stream, "%d %d %d", vec->x, vec->y, vec->z);
}

/*
 * print_node
 */

void print_node(FILE *stream, node_t *node, int n)
{
	fprintf(stream, "node: %d\n", n);
	fprintf(stream, "\tA: %0.0f\n", node->a);
	fprintf(stream, "\tB: %0.0f\n", node->b);
	fprintf(stream, "\tC: %0.0f\n", node->c);
	fprintf(stream, "\tD: %0.0f\n", node->d);
	fprintf(stream, "\tinid: %d\n", node->inid);
	fprintf(stream, "\toutid: %d\n", node->outid);
	fprintf(stream, "\n");
	fflush(stream);
}

/*
 * print_polygon
 */

void print_polygon(FILE *stream, polygon_t *polygon, int n)
{
	int i;

	/* idx */
	fprintf(stream, "polygon: %d\n", n);

	/* verts */
	fprintf(stream, "\tnum_verts: %d\n", polygon->num_verts);
	fprintf(stream, "\tverts: ");
	for (i = 0; i < polygon->num_verts; i++)
		fprintf(stream, "%d ", polygon->verts[i]);
	fprintf(stream, "\n");

	/* tname */
	fprintf(stream, "\ttname: %s\n", polygon->tname);

	/* tu */
	fprintf(stream, "\ttu: ");
	print_vec3(stream, &polygon->tu);
	fprintf(stream, "\n");

	/* tv */
	fprintf(stream, "\ttv: ");
	print_vec3(stream, &polygon->tv);
	fprintf(stream, "\n");

	/* to */
	fprintf(stream, "\tto: ");
	print_vec3(stream, &polygon->to);
	fprintf(stream, "\n");

	/* flush */
	fprintf(stream, "\n");
	fflush(stream);
}

/*
 * read_vec3i
 */

void read_vec3i(FILE *stream, vec3i_t *vec)
{
	/* variables */
	token_t token;

	token_read(stream, &token);
	vec->x = atoi(token.str);

	token_read(stream, &token);
	vec->y = atoi(token.str);

	token_read(stream, &token);
	vec->z = atoi(token.str);
}

/*
 * read_vec3
 */

void read_vec3(FILE *stream, vec3_t *vec)
{
	/* variables */
	token_t token;

	token_read(stream, &token);
	vec->x = atof(token.str);

	token_read(stream, &token);
	vec->y = atof(token.str);

	token_read(stream, &token);
	vec->z = atof(token.str);
}

/*
 * read_float
 */

void read_float(FILE *stream, float *f)
{
	token_t token;
	token_read(stream, &token);
	*f = atof(token.str);
}

/*
 * read_int
 */

void read_int(FILE *stream, int *i)
{
	token_t token;
	token_read(stream, &token);
	*i = atoi(token.str);
}

/*
 * read_string
 */

void read_string(FILE *stream, char *string, int maxlen)
{
	token_t token;
	token_read(stream, &token);
	strncpy(string, token.str, maxlen);
}

/*
 * read_polygon
 */

void read_polygon(FILE *stream, polygon_t *polygon, int n)
{
	/* blah */
	int i;
	token_t token;

	/* verts */
	i = 0;
	if (token_expect(stream, &token, "verts"))
	{
		/* tname is next */
		while (!token_expect(stream, &token, "tname"))
		{
			/* otherwise its a vert */
			polygon->verts[i++] = atoi(token.str);
		}

		polygon->num_verts = i;
	}
	else
	{
		printf("error: no verts in polygon\n");
	}

	/* tname */
	read_string(stream, polygon->tname, sizeof(polygon->tname));

	/* tu */
	if (token_expect(stream, &token, "tu"))
		read_vec3(stream, &polygon->tu);

	/* tv */
	if (token_expect(stream, &token, "tv"))
		read_vec3(stream, &polygon->tv);

	/* to */
	if (token_expect(stream, &token, "to"))
		read_vec3(stream, &polygon->to);

	/* assign node */
	polygon->node = n;
}

/*
 * read_node
 */

void read_node(bsp_t *bsp, FILE *stream, node_t *node, int n)
{
	token_t token;

	while (token_read(stream, &token))
	{
		/* A */
		if (token_string(&token, "A"))
			read_float(stream, &node->a);

		/* B */
		if (token_string(&token, "B"))
			read_float(stream, &node->b);

		/* C */
		if (token_string(&token, "C"))
			read_float(stream, &node->c);

		/* D */
		if (token_string(&token, "D"))
			read_float(stream, &node->d);

		/* inid */
		if (token_string(&token, "inid"))
			read_int(stream, &node->inid);

		/* outid */
		if (token_string(&token, "outid"))
			read_int(stream, &node->outid);

		/* front */
		if (token_string(&token, "front"))
			read_int(stream, &node->front);

		/* back */
		if (token_string(&token, "back"))
			read_int(stream, &node->back);

		/* polygon */
		if (token_string(&token, "polygon"))
		{
			int p;
			read_int(stream, &p);
			read_polygon(stream, &bsp->polygons[p], n);
		}

		/* next node */
		if (token_string(&token, "node"))
		{
			int n;
			read_int(stream, &n);
			read_node(bsp, stream, &bsp->nodes[n], n);
		}
	}
}

/*
 * bsp_read
 */

bsp_t *bsp_read(const char *filename)
{
	/* variables */
	FILE *file;
	token_t token;
	bsp_t *bsp;

	/* open file */
	file = fopen(filename, "rb");
	if (file == NULL)
	{
		printf("error: failed to open %s\n", filename);
		return NULL;
	}

	/* alloc */
	bsp = calloc(1, sizeof(bsp_t));
	if (bsp == NULL)
	{
		printf("error: failed malloc\n");
		return NULL;
	}

	/* token loop */
	while (token_read(file, &token))
	{
		/*
		 * read camera
		 */

		if (token_string(&token, "viewpoint"))
			read_vec3(file, &bsp->camera.viewpoint);

		if (token_string(&token, "viewnormal"))
			read_vec3(file, &bsp->camera.viewnormal);

		if (token_string(&token, "viewangle"))
			read_int(file, &bsp->camera.viewangle);

		if (token_string(&token, "texturelength"))
			read_int(file, &bsp->camera.texturelength);

		/* read xcomponents */
		if (token_string(&token, "xcomponents"))
		{
			int x;
			read_int(file, &bsp->num_xcomponents);
			bsp->xcomponents = calloc(bsp->num_xcomponents, sizeof(component_t));
			for (x = 0; x < bsp->num_xcomponents; x++) read_float(file, &bsp->xcomponents[x]);
			#if DEBUG
			printf("%d xcomponents read\n", bsp->num_xcomponents);
			#endif
		}

		/* read ycomponents */
		if (token_string(&token, "ycomponents"))
		{
			int y;
			read_int(file, &bsp->num_ycomponents);
			bsp->ycomponents = calloc(bsp->num_ycomponents, sizeof(component_t));
			for (y = 0; y < bsp->num_ycomponents; y++) read_float(file, &bsp->ycomponents[y]);
			#if DEBUG
			printf("%d ycomponents read\n", bsp->num_ycomponents);
			#endif
		}

		/* read zcomponents */
		if (token_string(&token, "zcomponents"))
		{
			int z;
			read_int(file, &bsp->num_zcomponents);
			bsp->zcomponents = calloc(bsp->num_zcomponents, sizeof(component_t));
			for (z = 0; z < bsp->num_zcomponents; z++) read_float(file, &bsp->zcomponents[z]);
			#if DEBUG
			printf("%d zcomponents read\n", bsp->num_zcomponents);
			#endif
		}

		/* read vertices */
		if (token_string(&token, "numverts"))
		{
			int v;

			read_int(file, &bsp->num_vertices);

			bsp->vertices = calloc(bsp->num_vertices, sizeof(vec3i_t));

			for (v = 0; v < bsp->num_vertices; v++)
			{
				read_vec3i(file, &bsp->vertices[v]);
			}

			#if DEBUG
			printf("%d vertices read\n", bsp->num_vertices);
			#endif
		}

		/* allocate nodes */
		if (token_string(&token, "numnodes"))
		{
			read_int(file, &bsp->num_nodes);
			bsp->nodes = calloc(bsp->num_nodes, sizeof(node_t));
			#if DEBUG
			printf("%d nodes read\n", bsp->num_nodes);
			#endif
		}

		/* allocate polygons */
		if (token_string(&token, "numpolys"))
		{
			read_int(file, &bsp->num_polygons);
			bsp->polygons = calloc(bsp->num_polygons, sizeof(polygon_t));
			#if DEBUG
			printf("%d poylgons read\n", bsp->num_polygons);
			#endif
		}

		/* read node */
		if (token_string(&token, "node"))
		{
			int n;
			read_int(file, &n);
			read_node(bsp, file, &bsp->nodes[n], n);
		}
	}

	/* close file */
	fclose(file);

	/* return ptr */
	return bsp;
}

/*
 * bsp_free
 */

void bsp_free(bsp_t *bsp)
{
	if (bsp)
	{
		if (bsp->xcomponents) free(bsp->xcomponents);
		if (bsp->ycomponents) free(bsp->ycomponents);
		if (bsp->zcomponents) free(bsp->zcomponents);
		if (bsp->vertices) free(bsp->vertices);
		if (bsp->polygons) free(bsp->polygons);
		if (bsp->nodes) free(bsp->nodes);

		free(bsp);
	}
}

/*
 * bsp_save
 */

void bsp_save(bsp_t *bsp, const char *filename)
{
	/* variables */
	FILE *file;
	int i;

	/* open file */
	file = fopen(filename, "wb");
	if (file == NULL)
	{
		printf("error: failed to open %s for writing\n", filename);
		return;
	}

	/* CAMERA */
	fprintf(file, "CAMERA\n");

	/* viewpoint */
	fprintf(file, "viewpoint ");
	print_vec3(file, &bsp->camera.viewpoint);
	fprintf(file, "\n");

	/* viewnormal */
	fprintf(file, "viewnormal ");
	print_vec3(file, &bsp->camera.viewnormal);
	fprintf(file, "\n");

	/* viewangle */
	fprintf(file, "viewangle %d\n", bsp->camera.viewangle);

	/* texturelength */
	fprintf(file, "texturelength %d\n", bsp->camera.texturelength);

	/* STRUCTURE */
	fprintf(file, "STRUCTURE\n");

	/* xcomponents */
	fprintf(file, "xcomponents %d\n", bsp->num_xcomponents);
	for (i = 0; i < bsp->num_xcomponents; i++)
		fprintf(file, "%0.6f\n", bsp->xcomponents[i]);

	/* ycomponents */
	fprintf(file, "ycomponents %d\n", bsp->num_ycomponents);
	for (i = 0; i < bsp->num_ycomponents; i++)
		fprintf(file, "%0.6f\n", bsp->ycomponents[i]);

	/* zcomponents */
	fprintf(file, "zcomponents %d\n", bsp->num_zcomponents);
	for (i = 0; i < bsp->num_zcomponents; i++)
		fprintf(file, "%0.6f\n", bsp->zcomponents[i]);

	/* vertices */
	fprintf(file, "numverts %d\n", bsp->num_vertices);
	for (i = 0; i < bsp->num_vertices; i++)
	{
		print_vec3i(file, &bsp->vertices[i]);
		fprintf(file, "\n");
	}

	/* numpolys */
	fprintf(file, "numpolys %d\n", bsp->num_polygons);

	/* BSPTREE */
	fprintf(file, "BSPTREE\n");

	/* numnodes */
	fprintf(file, "numnodes %d\n", bsp->num_nodes);

	/* TODO: nodes and polys */

	/* close file */
	fclose(file);
}
