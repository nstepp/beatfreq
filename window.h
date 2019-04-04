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

    $Id: window.h 613 2010-08-25 17:40:47Z stepp $
*/

#ifndef _WINDOW_H
#define _WINDOW_H

#include "global.h"

/* window dimensions:

         i -->
  _____________________
  |                   |
j |                   |
  |                   |
| |                   |
V |                   |
  |___________________|

*/


/*
This type describes a rectangular window of data
with data_size rows, and window_size columns.
data_sized data is stacked from left to right
until stack_point reaches window_size-1.
at which point we start scrolling and all
new data gets stacked at begin.
Then to actually scroll the data, begin
is incremented(%window_size) so that the
new data appears at the end of the window
*/
typedef struct window_t {
	data_t *data;
	size_t begin;
	size_t data_size;
	size_t window_size;
	int scrolling;
	size_t stack_point;
} window_t;

// Data index = begin + (i * window_size) + j)
#define DATA_IDX(b,w,i,j) ( ((b) + (i))%w + ((j) * (w)))

window_t *init_window(size_t data_size, size_t window_size);
void free_window(window_t *window);
void stack_window(window_t *window, data_t *data);
void avg_window(window_t *window,data_t *avg_data);
void copy_window_row(data_t *dest, window_t *window, int j);

#endif /* _WINDOW_H */
