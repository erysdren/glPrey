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
#include <SDL.h>
#include <SDL_opengles2.h>

/* gl */
#include <GL/gl.h>
#include <GL/glu.h>

/* backend */
#include "backend.h"

/*
 *
 * globals
 *
 */

/* sdl2 */
SDL_Window *window;
SDL_GLContext context;
const Uint8 *keys;

/* gl */
vec3_t m_pos;
vec3_t m_rot;
vec3_t m_look;
vec3_t m_strafe;
vec3_t m_vel;
vec3_t m_speedkey;
vec2_t mouse;
vec3i_t mb;

/*
 *
 * functions
 *
 */

/*
 * error
 */

void error(const char *s, ...)
{
	/* variables */
	va_list ap;
	static char scratch[256];

	/* do vargs */
	va_start(ap, s);
	vsnprintf(scratch, 256, s, ap);
	va_end(ap);

	/* show message box error */
	SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Error", scratch, NULL);

	/* print to stderr */
	fprintf(stderr, "error: %s\n", scratch);

	/* exit */
	exit(1);
}

/*
 * dot
 */

float dot(vec3_t v1, vec3_t v2)
{
	float result = 0.0f;
	result += v1.x * v2.x;
	result += v1.y * v2.y;
	result += v1.z * v2.z;
	return result;
}

/*
 * normalize
 */

float normalize(vec3_t *v)
{
	float w = sqrtf(v->x * v->x + v->y * v->y + v->z * v->z);
	v->x /= w;
	v->y /= w;
	v->z /= w;
	return w;
}

/*
 * camera
 */

void camera(float speed, float hfov)
{
	int w, h;
	float vfov;
	float aspect;

	SDL_GL_GetDrawableSize(window, &w, &h);

	/* speed */
	if (key(SDL_SCANCODE_LSHIFT))
	{
		m_speedkey.x = 2;
		m_speedkey.y = 2;
		m_speedkey.z = 2;
	}
	else
	{
		m_speedkey.x = 1;
		m_speedkey.y = 1;
		m_speedkey.z = 1;
	}

	/* forwards */
	if (key(SDL_SCANCODE_W))
	{
		m_pos.x += m_look.x * speed * m_speedkey.x;
		m_pos.y += m_look.y * speed * m_speedkey.y;
		m_pos.z += m_look.z * speed * m_speedkey.z;
	}

	/* backwards */
	if (key(SDL_SCANCODE_S))
	{
		m_pos.x -= m_look.x * speed * m_speedkey.x;
		m_pos.y -= m_look.y * speed * m_speedkey.y;
		m_pos.z -= m_look.z * speed * m_speedkey.z;
	}

	/* left */
	if (key(SDL_SCANCODE_A))
	{
		m_pos.x += m_strafe.x * speed * m_speedkey.x;
		m_pos.z += m_strafe.z * speed * m_speedkey.z;
	}

	/* right */
	if (key(SDL_SCANCODE_D))
	{
		m_pos.x -= m_strafe.x * speed * m_speedkey.x;
		m_pos.z -= m_strafe.z * speed * m_speedkey.z;
	}

	/* arrow keys */
	if (key(SDL_SCANCODE_UP)) m_rot.x += 8.0f * speed;
	if (key(SDL_SCANCODE_DOWN)) m_rot.x -= 8.0f * speed;
	if (key(SDL_SCANCODE_LEFT)) m_rot.y -= 8.0f * speed;
	if (key(SDL_SCANCODE_RIGHT)) m_rot.y += 8.0f * speed;

	/* mouse look */
	if (mb.x || mb.y || mb.z)
	{
		m_rot.x -= mouse.y * speed;
		m_rot.y += mouse.x * speed;
	}

	/* lock camera */
	if (m_rot.x < -75.0f) m_rot.x = -75.0f;
	if (m_rot.x > 75.0f) m_rot.x = 75.0f;

	/* set viewport */
	glViewport(0, 0, w, h);

	/* get fov and aspect */
	aspect = (float)w / (float)h;
	vfov = 2 * atanf(tanf(DEG2RAD(hfov) / 2) * (float)h / (float)w);

	/* set perspective */
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(ceilf(RAD2DEG(vfov)), aspect, 1, FLT_MAX);

	/* set camera view */
	m_look.x = cosf(DEG2RAD(m_rot.y)) * cosf(DEG2RAD(m_rot.x));
	m_look.y = sinf(DEG2RAD(m_rot.x));
	m_look.z = sinf(DEG2RAD(m_rot.y)) * cosf(DEG2RAD(m_rot.x));
	m_strafe.x = cosf(DEG2RAD(m_rot.y) - M_PI_2);
	m_strafe.z = sinf(DEG2RAD(m_rot.y) - M_PI_2);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(m_pos.x, m_pos.y, m_pos.z, m_pos.x + m_look.x,
		m_pos.y + m_look.y, m_pos.z + m_look.z, 0.0f, 1.0f, 0.0);
}

/*
 * camera_set_pos
 */

void camera_set_pos(float x, float y, float z)
{
	m_pos.x = x;
	m_pos.y = y;
	m_pos.z = z;
}

/*
 * frame
 */

bool frame(void)
{
	SDL_Event event;
	bool ret = true;

	mouse.x = 0;
	mouse.y = 0;

	while (SDL_PollEvent(&event))
	{
		switch (event.type)
		{
			case SDL_QUIT:
				ret = false;
				break;

			case SDL_MOUSEBUTTONDOWN:
				SDL_SetRelativeMouseMode(SDL_TRUE);
				if (event.button.button == SDL_BUTTON_LEFT)
					mb.x = 1;
				else if (event.button.button == SDL_BUTTON_RIGHT)
					mb.y = 1;
				else if (event.button.button == SDL_BUTTON_MIDDLE)
					mb.z = 1;
				break;

			case SDL_MOUSEBUTTONUP:
				SDL_SetRelativeMouseMode(SDL_FALSE);
				if (event.button.button == SDL_BUTTON_LEFT)
					mb.x = 0;
				else if (event.button.button == SDL_BUTTON_RIGHT)
					mb.y = 0;
				else if (event.button.button == SDL_BUTTON_MIDDLE)
					mb.z = 0;
				break;

			case SDL_MOUSEMOTION:
				mouse.x += event.motion.xrel;
				mouse.y += event.motion.yrel;
				break;
		}
	}

	keys = SDL_GetKeyboardState(NULL);

	SDL_GL_SwapWindow(window);

	glClearColor(0, 0, 0, 1);
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

	return ret;
}

/*
 * init
 */

bool init(int w, int h, char *title)
{
	/* sdl */
	SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS);
	window = SDL_CreateWindow(title, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, w, h, SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE | SDL_WINDOW_SHOWN);
	if (window == NULL) return false;

	/* gl */
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 1);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_COMPATIBILITY);
	context = SDL_GL_CreateContext(window);
	if (context == NULL) return false;

	/* swap interval */
	SDL_GL_SetSwapInterval(1);

	/* enable gl features */
	glEnable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);

	/* exit gracefully */
	return true;
}

/*
 * quit
 */

void quit(void)
{
	SDL_DestroyWindow(window);
	SDL_GL_DeleteContext(context);
	SDL_Quit();
}

/*
 * key
 */

bool key(int sc)
{
	return keys[sc] ? true : false;
}

/*
 * zalloc
 */

void *zalloc(size_t size)
{
	return calloc(1, size);
}
