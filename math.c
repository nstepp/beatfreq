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

    $Id: math.c 925 2016-04-07 16:42:02Z stepp $
*/

#include <math.h>
#include <sys/types.h>
#include <fftw3.h>
#include "global.h"
#include "math.h"
#include "window.h"

/*
 * Just a wrapper around the FFTW create plan function
 */
/*
fftw_plan init_fft(size_t n)
{
	return rfftw_create_plan(n,FFTW_REAL_TO_COMPLEX,FFTW_ESTIMATE);
}
*/

/*
 * This applies a hanning window to a data array.
 * A hanning window is a simple window which multiplies the
 * data set with a sinusoid which is 0 at the endpoints and
 * 1 at the center point.
 */
void apply_hanning_window(data_t *data_ptr, int n)
{
	int i;
	
	for(i=0;i<n;i++) {
		*(data_ptr++) *= 0.5*cos(M_PI*( (2.0*(data_t)i)/(data_t)n + 1.0 )) + 0.5;
	}
}

/*
 * This applies a hanning window to a data window.
 * A hanning window is a simple window which multiplies the
 * data set with a sinusoid which is 0 at the endpoints and
 * 1 at the center point.
 */
void wapply_hanning_window(window_t *window, int j)
{
	int i;
	data_t *data_ptr = window->data + (j * window->window_size);

	
	for(i=0;i<window->window_size;i++) {
		*(data_ptr++) *= 0.5*cos(M_PI*( (2.0*(data_t)i)/(data_t)window->window_size + 1.0 )) + 0.5;
	}
}

/*
 * Just a simple normalizing function.  Normalizes to 1
 */
void normalize_data_1d(data_t *data, size_t n)
{
	size_t i;
	data_t max=0.0;

	/* find max */
	for(i=0;i<n;i++) {
		if(data[i]>max) {
			max=data[i];
		}
	}

	/* normalize by max */
	for(i=0;i<n;i++) {
		data[i]=data[i]/max;
	}
}

/*
 * Calculate the power spectrum from a normal spectrum.
 * All that we are doing is performing a complex conjugate
 */
void power_spectrum(data_t *pspec, data_t *raw_fft, size_t data_size)
{
	int k;
		
	pspec[0] = raw_fft[0]*raw_fft[0];
	for(k=1;k<(data_size+1)/2;++k) {
		pspec[k] = raw_fft[k]*raw_fft[k] + raw_fft[data_size-k]*raw_fft[data_size-k];
	}
	pspec[data_size/2] = raw_fft[data_size/2]*raw_fft[data_size/2];  /* Nyquist freq. */
}

void create_maxima_list(data_t *data, size_t window_size, maxima_list_t *list, data_t thresh)
{
	int cur_idx    = 0;
	data_t maximum = 0.0;

	while(maximum != -1.0) {
		maximum = next_local_maximum(&cur_idx,data,window_size,thresh);
		if(maximum >= thresh) {
			list = add_maximum(list,cur_idx,maximum);
		}
	}
	
}

/*
 * This is a *very* simple peak finding algorithm.  All it does is this:
 * Watch the derivative of the data.  When it goes above some threshold
 * value, start coloring the data red.  If the slope goes negative after
 * it has gone red, then mark the last point before the negative slope
 * as the peak.
 *
 * Additionally, disregard very small peaks, which are defined as peaks
 * with intensities below another threshold.
 *
 * Save the peaks in a list, which we return.
 */
maxima_list_t *find_peaks(data_t *data, size_t data_size)
{
	int i;
	int red=0;
	data_t intens,last_intens;
	data_t slope;
	maxima_list_t *peak_info = NULL;

	last_intens = data[0];
	
	for(i = 1; i < data_size; i++) {
		intens = data[i];

		slope = (intens - last_intens)/BEAT_FREQ_BAND;

		if( slope > PEAK_SLOPE_THRESH ) {
			red = 1;
		} else if( red && (slope < 0) && (last_intens > PEAK_INTENS_THRESH)) {
			peak_info = add_maximum(peak_info,i-1,last_intens);
			red = 0;
		}

		last_intens = intens;
	}

	return peak_info;
}

data_t next_local_maximum(int *cur_idx,data_t *data,size_t window_size,data_t thresh)
{
	int i   = *cur_idx;
	int inc = 0;
	data_t sample,last_sample,dxdy=0;

	last_sample = data[i];

	for(i=i+1;i<window_size;i++) {
		sample = data[i];
		if(sample > last_sample) {
			inc = 1;
			dxdy = sample - last_sample;
		} else if (inc && (sample < last_sample) && (dxdy > thresh)) {
			*cur_idx = i-1;

			return(last_sample);
		}
	}

	*cur_idx = -1;
	return(-1.0);
}


