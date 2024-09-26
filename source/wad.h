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

/* wad header */
typedef struct
{
	char magic[4];
	int32_t num_lumps;
	int32_t ofs_lumps;
} wad_header_t;

/* wad lump */
typedef struct
{
	int32_t ofs_data;
	int32_t len_data;
	int32_t type;
	char name[8];
	void *data;
} wad_lump_t;

/* wad structure */
typedef struct
{
	wad_header_t header;
	wad_lump_t *lumps;
} wad_t;

/* function prototypes */
wad_t *wad_read(const char *filename);
void wad_free(wad_t *wad);
void *wad_find(wad_t *wad, const char *search, int *size);
