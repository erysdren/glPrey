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

/* mip header */
typedef struct
{
	uint8_t width;
	uint8_t height;
	uint8_t num_entries;
	uint8_t pad;
} mip_header_t;

/* miptex */
typedef struct
{
	uint8_t width;
	uint8_t height;
	uint8_t *pixels;
} mip_entry_t;

/* mip structure */
typedef struct
{
	mip_header_t header;
	mip_entry_t *entries;
} mip_t;

/* function prototypes */
mip_t *mip_from_buffer(void *buffer, size_t buffer_len);
mip_t *mip_from_file(const char *filename);
void *mip_free(mip_t *mip);
