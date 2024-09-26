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

/* backend */
#include "backend.h"

/* bsp camera */
typedef struct
{
	vec3_t viewpoint;
	vec3_t viewnormal;
	int viewangle;
	int texturelength;
} camera_t;

#define TOKEN_STR_LEN 128

/* lexer token */
typedef struct
{
	char str[TOKEN_STR_LEN];
	int len;
} token_t;

/* bsp x/y/z component */
typedef float component_t;

/* bsp polygon */
typedef struct
{
	int verts[32];
	int num_verts;
	char tname[32];
	vec3_t tu;
	vec3_t tv;
	vec3_t to;
	int node;
} polygon_t;

/* bsp node */
typedef struct
{
	float a, b, c, d;
	int inid;
	int outid;
	int front;
	int back;
} node_t;

/* bsp structure */
typedef struct
{
	camera_t camera;

	component_t *xcomponents;
	int num_xcomponents;

	component_t *ycomponents;
	int num_ycomponents;

	component_t *zcomponents;
	int num_zcomponents;

	vec3i_t *vertices;
	int num_vertices;

	polygon_t *polygons;
	int num_polygons;

	node_t *nodes;
	int num_nodes;
} bsp_t;

/* function prototypes */
bsp_t *bsp_read(const char *filename);
void bsp_free(bsp_t *bsp);
void bsp_save(bsp_t *bsp, const char *filename);
