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

/* stb_image */
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

/* glprey */
#include "wad.h"
#include "mip.h"

/*
 *
 * functions
 *
 */

/*
 * write_palette
 */

void write_palette(wad_lump_t *lump)
{
	char filename[16];
	snprintf(filename, 16, "%s.png", lump->name);
	stbi_write_png(filename, 16, 16, 3, lump->data, 16 * 3);
	printf("successfully wrote %s\n", filename);
}

/*
 * write_colormap
 */

void write_colormap(wad_lump_t *lump, uint8_t *palette)
{
	/* variables */
	char filename[16];
	uint8_t *colormap;
	uint8_t *pixels;
	int num_pixels;
	int i;

	/* get ptr */
	colormap = (uint8_t *)lump->data + 8;

	/* create 24 bit version */
	num_pixels = 256 * 32;
	pixels = malloc(num_pixels * 3);
	for (i = 0; i < num_pixels; i++)
	{
		uint8_t *entry = &((uint8_t *)palette)[colormap[i] * 3];
		pixels[i * 3] = *(entry);
		pixels[(i * 3) + 1] = *(entry + 1);
		pixels[(i * 3) + 2] = *(entry + 2);
	}

	/* write file */
	snprintf(filename, 16, "%s.png", lump->name);
	stbi_write_png(filename, 256, 32, 3, pixels, 256 * 3);

	/* talk to you */
	printf("successfully wrote %s\n", filename);

	/* free memory */
	free(pixels);
}

/*
 * write_mip
 */

void write_mip(wad_lump_t *lump, uint8_t *palette)
{
	mip_t *mip;
	int num_pixels;
	char filename[16];
	uint8_t *pixels;
	int i;

	/* get miptex */
	mip = mip_from_buffer(lump->data, lump->len_data);
	num_pixels = mip->header.width * mip->header.height;

	/* create 24 bit version */
	pixels = malloc(num_pixels * 3);
	for (i = 0; i < num_pixels; i++)
	{
		uint8_t *entry = &((uint8_t *)palette)[mip->entries[0].pixels[i] * 3];
		pixels[i * 3] = *(entry);
		pixels[(i * 3) + 1] = *(entry + 1);
		pixels[(i * 3) + 2] = *(entry + 2);
	}

	/* save image */
	snprintf(filename, 16, "%s.png", lump->name);
	stbi_write_png(filename, mip->header.width, mip->header.height, 3, pixels, mip->header.width * 3);

	/* talk to you */
	printf("successfully wrote %s\n", filename);

	/* free mip */
	free(pixels);
	mip_free(mip);
}

/*
 * main
 */

int main(int argc, char *argv[])
{
	/* variables */
	wad_t *wad;
	uint8_t *palette;
	int i;

	/* read wad */
	if (argc > 1)
	{
		wad = wad_read(argv[1]);
		if (!wad)
		{
			printf("error: failed to open %s\n", argv[1]);
			return 1;
		}
	}
	else
	{
		wad = wad_read("MACT.WAD");
		if (!wad)
		{
			printf("error: failed to open MACT.WAD\n");
			return 1;
		}
	}

	/* get palette */
	palette = wad_find(wad, "PAL", NULL);
	if (!palette)
	{
		printf("error: wad does not contain PAL lump\n");
		return 1;
	}

	/* read lumps */
	for (i = 0; i < wad->header.num_lumps; i++)
	{
		switch (wad->lumps[i].type)
		{
			case 8:
				write_palette(&wad->lumps[i]);
				break;

			case 11:
				write_mip(&wad->lumps[i], palette);
				break;

			case 17:
				write_colormap(&wad->lumps[i], palette);
				break;

			default:
				printf("lump %s has unsupported type %d\n", wad->lumps[i].name, wad->lumps[i].type);
				break;
		}
	}

	/* free memory */
	wad_free(wad);

	/* return success */
	return 0;
}
