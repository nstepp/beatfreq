/*
	beatfreq - FFT-based Beat frequency calculator

    Copyright (C) 2003-2010  Nigel D. Stepp

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License along
    with this program; if not, write to the Free Software Foundation, Inc.,
    51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.

	This program is DUAL LICENSED under the MIT License, which means you may
	choose the license terms appropriate for your use. See COPYING-GPL and
	COPYING-MIT for details.

    Nigel Stepp <stepp@atistar.net>

    $Id: graphics.h 613 2010-08-25 17:40:47Z stepp $
*/

#ifndef _GRAPHICS_H
#define _GRAPHICS_H

#include <sys/types.h>
#include <ggi/ggi.h>
#include "global.h"
#include "window.h"
#include "list.h"

/*
 * keep visual state info
 */
typedef struct visual_t {
	ggi_visual_t vis;
	int depth;
	int sx;
	int sy;
	int vx;
	int vy;
} visual_t;

/*
 * visualization state info
 */
typedef struct vis_scope_t {
	float x_stretch_factor;
	ggi_pixel black;
	ggi_pixel fgcolor;
	ggi_pixel fgcolor2;
	data_t dy;
	int lastx;
	int lasty;
} vis_scope_t;

visual_t *init_visual(int x, int y);
vis_scope_t *visualize_scope_init(visual_t *visual, size_t data_size, data_t ymax);
vis_scope_t *visualize_sgraph_init(visual_t *visual,size_t data_size, data_t ymax);
void visualize_sgraph_step(visual_t *visual, vis_scope_t *scope_data, data_t *data, maxima_list_t *peak_info, size_t data_size);
void visualize_scope_step(visual_t *visual, vis_scope_t *scope_data, data_t *data, size_t data_size);
void visualize_beat_graph(visual_t *visual, window_t *beat_window, size_t window_size, size_t data_size);
void visualize_window(visual_t *visual, window_t *window, size_t window_size, size_t data_size);


#endif /* _GRAPHICS_H */
