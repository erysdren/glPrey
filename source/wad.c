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
#include <stdint.h>

/* wad */
#include "wad.h"

/*
 *
 * functions
 *
 */

/*
 * wad_read
 */

wad_t *wad_read(const char *filename)
{
	/* variables */
	FILE *file;
	int i;
	wad_t *wad;

	/* open file */
	file = fopen(filename, "rb");
	if (file == NULL)
	{
		printf("error: failed to open %s\n", filename);
		return NULL;
	}

	/* alloc */
	wad = calloc(1, sizeof(wad_t));
	if (wad == NULL)
	{
		printf("error: failed malloc\n");
		return NULL;
	}

	/* read header */
	fread(&wad->header, sizeof(wad_header_t), 1, file);

	/* check magic */
	if (memcmp(&wad->header.magic, "IWAD", 4) != 0)
	{
		printf("error: invalid wad file\n");
		return NULL;
	}

	/* allocate lumps */
	wad->lumps = calloc(wad->header.num_lumps, sizeof(wad_lump_t));
	if (wad->lumps == NULL)
	{
		printf("error: failed malloc\n");
		return NULL;
	}

	/* read lumps */
	fseek(file, wad->header.ofs_lumps, SEEK_SET);
	for (i = 0; i < wad->header.num_lumps; i++)
	{
		fread(&wad->lumps[i].ofs_data, sizeof(int32_t), 1, file);
		fread(&wad->lumps[i].len_data, sizeof(int32_t), 1, file);
		fread(&wad->lumps[i].type, sizeof(int32_t), 1, file);
		fread(&wad->lumps[i].name, sizeof(char), 8, file);
	}

	/* read lump data */
	for (i = 0; i < wad->header.num_lumps; i++)
	{
		/* alloc */
		wad->lumps[i].data = malloc(wad->lumps[i].len_data);
		if (wad->lumps[i].data == NULL)
		{
			printf("error: failed malloc\n");
			return NULL;
		}

		/* read */
		fseek(file, wad->lumps[i].ofs_data, SEEK_SET);
		fread(wad->lumps[i].data, wad->lumps[i].len_data, 1, file);
	}

	/* close file */
	fclose(file);

	/* return ptr */
	return wad;
}

/*
 * wad_free
 */

void wad_free(wad_t *wad)
{
	int i;

	if (wad)
	{
		if (wad->lumps)
		{
			for (i = 0; i < wad->header.num_lumps; i++)
			{
				if (wad->lumps[i].data)
					free(wad->lumps[i].data);
			}

			free(wad->lumps);
		}

		free(wad);
	}
}

/*
 * wad_find
 */

void *wad_find(wad_t *wad, const char *search, int *size)
{
	/* variables */
	int i;

	/* search */
	for (i = 0; i < wad->header.num_lumps; i++)
	{
		if (strcmp(search, wad->lumps[i].name) == 0)
		{
			if (size) *size = wad->lumps[i].len_data;
			return wad->lumps[i].data;
		}
	}

	/* failure */
	if (size) *size = 0;
	return NULL;
}
