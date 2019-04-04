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

    $Id: window.c 613 2010-08-25 17:40:47Z stepp $
*/

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <string.h>
#include "window.h"
#include "global.h"

/*
 * A "window" as defined in this file is a block of
 * data which has a data size and window size.
 *
 * It can be thought of as a series of data sized
 * objects stacked so that there is a window sized
 * collection of them.
 *
 * Visually:
 * 
 *      _____window_size____
 *     |                    |
 *     |                    |
 * data_size                |
 *     |                    |
 *     |____________________|
 * 
 * Furthermore, new data can be added to the end by
 * scrolling old data out of the window.
 */

void free_window(window_t *window)
{
	free(window->data);
	free(window);
}

/*
 * Sets up a window, which includes allocating the
 * data
 */
window_t *init_window(size_t data_size, size_t window_size)
{
	window_t *window = malloc(sizeof(window_t));

	window->data        = calloc(data_size*window_size,sizeof(data_t));
	window->begin       = 0;
	window->data_size   = data_size;
	window->window_size = window_size;
	window->scrolling   = 0;
	window->stack_point = 0;
	
	return(window);
}

/*
 * Add some data to a window, scroll if necessary
 */
void stack_window(window_t *window, data_t *data)
{
	int j;

	// Are we scrolling yet?
	if(window->scrolling) {
		// stack data at current beginning of window
		for(j=0;j<window->data_size;j++) {
			window->data[window->begin + (j*window->window_size)] = data[j];
		}

		// scroll window, now the data we just stacked is at end
		window->begin = (window->begin+1) % window->window_size;
	} else {
		// stack data at current stacking point
		for(j=0;j<window->data_size;j++) {
			window->data[window->stack_point + (j*window->window_size)] = data[j];
		}

		// if we have reached the end of the window, start scrolling
		if(++window->stack_point == (window->window_size - 1)) {
			window->scrolling = 1;
			window->begin = window->stack_point;
		}
	}
}

/*
 * Average each data_sized object in the window so that
 * we are left with a single window_sized object.
 *
 * XXX This is, by far, the slowest function in the program XXX
 * 
 */
void avg_window(window_t *window, data_t *avg_data)
{
	int i,j;
	data_t sum;
	data_t *column_head_ptr,*cell_ptr;

	column_head_ptr = window->data;
	for(i=0;i<window->window_size;i++) {
		sum=0.0;
		cell_ptr = column_head_ptr;
		for(j=0;j<window->data_size;j++) {
			sum+=*cell_ptr;
			cell_ptr += window->window_size;
		}
		avg_data[i]=sum/((data_t)window->data_size);
		column_head_ptr++;
	}
}

/*
 * Copy a window "row" from the window to an arbitrary
 * point in memory
 */
void copy_window_row(data_t *dest, window_t *window, int j)
{
	int i;
	data_t *data_ptr = window->data + (j * window->window_size);

	for(i=0;i<window->window_size;i++) {
		dest[i] = *(data_ptr++);
	}
}

