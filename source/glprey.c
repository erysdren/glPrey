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
#include <stdint.h>
#include <stdlib.h>
#include <float.h>
#include <math.h>
#include <string.h>
#include <stdbool.h>

/* sdl2 */
#include "SDL.h"
#include "SDL_opengl.h"

/* gl */
#include <GL/gl.h>
#include <GL/glu.h>

/* backend */
#include "backend.h"

/* glprey */
#include "wad.h"
#include "mip.h"
#include "bsp.h"

/*
 *
 * macros
 *
 */

/* level scale */
#define SCALE (1.0f/1024.0f)

/*
 *
 * globals
 *
 */

/* gl */
GLint gl_bsp;
gl_texture_t gl_textures[128];
int num_gl_textures = 0;
bool wireframe = false;

/*
 *
 * functions
 *
 */

/*
 * find_texture
 */

int find_texture(char *s)
{
	int i;

	for (i = 0; i < num_gl_textures; i++)
	{
		if (strcmp(gl_textures[i].name, s) == 0)
			return gl_textures[i].id;
	}

	error("couldn't find texture %s\n", s);

	return -1;
}

/*
 * process_bsp
 */

void process_bsp(bsp_t *bsp)
{
	/* variables */
	int i, v;

	/* open list */
	gl_bsp = glGenLists(1);
	glNewList(gl_bsp, GL_COMPILE);

	glShadeModel(GL_FLAT);

	/* do commands */
	for (i = 0; i < bsp->num_polygons; i++)
	{
		/* variables */
		vec3_t tu, tv, to;

		/* find texture */
		v = find_texture(bsp->polygons[i].tname);

		glBindTexture(GL_TEXTURE_2D, v);
		glEnable(GL_TEXTURE_2D);
		glBegin(GL_TRIANGLE_FAN);

		/* copy values */
		tu = bsp->polygons[i].tu;
		tv = bsp->polygons[i].tv;
		to = bsp->polygons[i].to;

		for (v = 0; v < bsp->polygons[i].num_verts; v++)
		{
			float s, t;
			vec3_t p, tp, nu, nv;

			p.x = bsp->xcomponents[bsp->vertices[bsp->polygons[i].verts[v]].x];
			p.y = bsp->ycomponents[bsp->vertices[bsp->polygons[i].verts[v]].y];
			p.z = bsp->zcomponents[bsp->vertices[bsp->polygons[i].verts[v]].z];

			/* calc offset */
			tp.x = (p.x - to.x);
			tp.y = (p.y - to.y);
			tp.z = (p.z - to.z);

			/* normalize */
			nu = tu;
			nv = tv;
			normalize(&nu);
			normalize(&nv);

			/* get s,t */
			s = dot(tp, nu) / 8;
			t = dot(tp, nv) / 8;

			/* yeah */
			s *= SCALE;
			t *= SCALE;
			p.x *= SCALE;
			p.y *= SCALE;
			p.z *= SCALE;

			/* do commands */
			glTexCoord2f(s, t);
			glVertex3f(p.x, p.y, p.z);
		}

		/* end gl */
		glEnd();
		glDisable(GL_TEXTURE_2D);
	}

	/* close list */
	glEndList();

	/* set camera pos */
	camera_set_pos(
		bsp->camera.viewpoint.x * SCALE,
		bsp->camera.viewpoint.y * SCALE,
		bsp->camera.viewpoint.z * SCALE
	);
}

/*
 * process_wad
 */

void process_wad(wad_t *wad)
{
	int i, p;
	mip_t *mip;
	int num_pixels;
	uint8_t *palette;

	palette = wad_find(wad, "PAL", NULL);

	/* generate textures */
	for (i = 0; i < wad->header.num_lumps; i++)
	{
		/* only miptex */
		if (wad->lumps[i].type != 11)
			continue;

		/* get mip */
		mip = mip_from_buffer(wad->lumps[i].data, wad->lumps[i].len_data);
		if (!mip)
		{
			printf("couldn't read mip %s\n", wad->lumps[i].name);
			continue;
		}

		/* copy values for searching */
		memcpy(gl_textures[num_gl_textures].name, wad->lumps[i].name, 8);
		gl_textures[num_gl_textures].width = mip->header.width;
		gl_textures[num_gl_textures].height = mip->header.height;
		num_pixels = gl_textures[num_gl_textures].width * gl_textures[num_gl_textures].height;

		/* create 24 bit version */
		gl_textures[num_gl_textures].pixels = malloc(num_pixels * 3);
		for (p = 0; p < num_pixels; p++)
		{
			uint8_t *entry = &((uint8_t *)palette)[mip->entries[0].pixels[p] * 3];
			gl_textures[num_gl_textures].pixels[p * 3] = *(entry);
			gl_textures[num_gl_textures].pixels[(p * 3) + 1] = *(entry + 1);
			gl_textures[num_gl_textures].pixels[(p * 3) + 2] = *(entry + 2);
		}

		glGenTextures(1, &gl_textures[num_gl_textures].id);
		glBindTexture(GL_TEXTURE_2D, gl_textures[num_gl_textures].id);
		glTexImage2D(GL_TEXTURE_2D, 0, 3, gl_textures[num_gl_textures].width, gl_textures[num_gl_textures].height, 0, GL_RGB, GL_UNSIGNED_BYTE, gl_textures[num_gl_textures].pixels);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

		/* number of gl textures */
		num_gl_textures++;

		/* free miptex */
		mip_free(mip);
	}
}

/*
 * main
 */

int main(int argc, char *argv[])
{
	int i;
	float time = 0.0f;
	Uint64 time_current, time_last;
	float deltatime;
	bsp_t *bsp = NULL;
	wad_t *wad = NULL;

	/* check if user specified files */
	for (i = 1; i < argc; i++)
	{
		/* bsp */
		if (strcmp(argv[i], "--bsp") == 0 && i + 1 < argc)
		{
			bsp = bsp_read(argv[i + 1]);
			if (!bsp) error("couldn't read bsp %s", argv[i + 1]);
		}

		/* wad */
		if (strcmp(argv[i], "--wad") == 0 && i + 1 < argc)
		{
			wad = wad_read(argv[i + 1]);
			if (!wad) error("couldn't read wad %s", argv[i + 1]);
		}
	}

	/* read them again if user didn't select */
	if (bsp == NULL) bsp = bsp_read("DEMO4.BSP");
	if (bsp == NULL) error("couldn't read bsp DEMO4.BSP");
	if (wad == NULL) wad = wad_read("MACT.WAD");
	if (wad == NULL) error("couldn't read wad MACT.WAD");

	/* init sdl and gl  */
	init(640, 480, "glPrey");

	/* print gl info */
	fprintf(stderr, "%s\n", glGetString(GL_VERSION));
	fprintf(stderr, "%s\n", glGetString(GL_RENDERER));

	/* init wad */
	process_wad(wad);

	/* init bsp */
	process_bsp(bsp);

	/* start time */
	time_last = SDL_GetTicks64();

	/* main loop */
	while (frame())
	{
		/* current frame time */
		time_current = SDL_GetTicks64();
		deltatime = (time_current - time_last) / 1000.0f;

		/* inputs */
		if (key(SDL_SCANCODE_ESCAPE))
			break;

		/* do camera */
		camera(32 * deltatime, 90.0f);

		/* other inputs */
		if (time > 0)
		{
			time -= 1.0f;
		}
		if (key(SDL_SCANCODE_TAB) && time < 1)
		{
			wireframe = wireframe ? false : true;
			time += 10.0f;
		}

		/* render map, optionally with wireframe */
		glPushMatrix();
		if (wireframe == true)
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		else
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		glCallList(gl_bsp);
		glPopMatrix();

		/* update frame time */
		time_last = time_current;
	}

	/* free textures */
	for (i = 0; i < num_gl_textures; i++)
	{
		if (gl_textures[i].pixels)
			free(gl_textures[i].pixels);
	}

	/* quit */
	glDeleteLists(gl_bsp, 1);
	bsp_free(bsp);
	wad_free(wad);
	quit();

	/* exit gracefully */
	return 0;
}
