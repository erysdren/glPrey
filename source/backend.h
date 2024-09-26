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

#ifndef _BACKEND_H_
#define _BACKEND_H_

/* std */
#include <stdbool.h>
#include <stddef.h>

/* gl */
#include <GL/gl.h>

/* vec3 (float) */
typedef struct
{
	float x, y, z;
} vec3_t;

/* vec3 (int) */
typedef struct
{
	int x, y, z;
} vec3i_t;

/* vec2 (float) */
typedef struct
{
	float x, y;
} vec2_t;

/* vec2 (int) */
typedef struct
{
	int x, y;
} vec2i_t;

/* gl texture */
typedef struct
{
	GLuint id;
	int width;
	int height;
	uint8_t *pixels;
	char name[16];
} gl_texture_t;

/* pi */
#ifndef M_PI
#define M_PI 3.14159265
#endif
#ifndef M_PI_2
#define M_PI_2 M_PI / 2
#endif

/* radians/deg */
#ifndef RAD2DEG
#define RAD2DEG(x) ((x) * 180.0f/M_PI)
#endif
#ifndef DEG2RAD
#define DEG2RAD(x) ((x) * M_PI/180.0f)
#endif

/* functions */
void error(const char *s, ...);
float dot(vec3_t v1, vec3_t v2);
float normalize(vec3_t *v);
void camera(float speed, float hfov);
void camera_set_pos(float x, float y, float z);
bool frame(void);
bool init(int w, int h, char *title);
void quit(void);
bool key(int sc);
void *zalloc(size_t size);

#endif /* _BACKEND_H_ */
