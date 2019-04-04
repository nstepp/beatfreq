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

    $Id: math.h 925 2016-04-07 16:42:02Z stepp $
*/

#ifndef __MATH_H
#define __MATH_H

#include <fftw3.h>
#include "global.h"
#include "window.h"
#include "list.h"

//fftw_plan init_fft(size_t n);
void normalize_data_1d(data_t *data, size_t n);
void normalize_data_2d(data_t *data, size_t rows, size_t cols);
void power_spectrum(data_t *pspec, data_t *raw_fft, size_t data_size);
data_t next_local_maximum(int *cur_idx,data_t *data,size_t window_size,data_t thresh);
maxima_list_t *add_maximum(maxima_list_t *list,int index, data_t intensity);
maxima_list_t *find_peaks(data_t *data, size_t data_size);
void create_maxima_list(data_t *data,size_t window_size,maxima_list_t *list,data_t thresh);
void apply_hanning_window(data_t *data_ptr, int n);
void wapply_hanning_window(window_t *window, int j);

#endif /* _MATH_H */
