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

    $Id: global.h 925 2016-04-07 16:42:02Z stepp $
*/

#ifndef _GLOBAL_H
#define _GLOBAL_H

#define EVER ;;

//#if (SIZEOF_FFTW_REAL == SIZEOF_DOUBLE)
typedef double data_t;
//#elif (SIZEOF_FFTW_REAL == SIZEOF_FLOAT)
// typedef float data_t;
//#else
// #error "fftw_real is neither double nor float, I don't know what to do." 
//#endif

// Most of these will/should be replaced with command line options
#define SAMPLE_SIZE 2048
#define WINDOW_SIZE 512
#define ZERO_PAD_SIZE 512
#define PEAK_SLOPE_THRESH 4
#define PEAK_INTENS_THRESH 0.5
#define NYQUIST_SIZE(a) ((a)/2 + 1)

#define SAMPLE_FREQ 44100

// temporary math stuff
#define FREQ_BANDWIDTH ((float)(SAMPLE_FREQ) / (float)(SAMPLE_SIZE))
#define SAMPLE_TIME ((float)(SAMPLE_SIZE) / (float)(SAMPLE_FREQ))
#define NYQUIST_INDEX(a) ((a) / 2) 
#define NYQUIST_FREQ ((float)(SAMPLE_FREQ) / 2)
#define WINDOW_TIME ((float)(SAMPLE_TIME) * (float)(WINDOW_SIZE))
#define LOW_CUTOFF ((FREQ_BANDWIDTH) * 2)
#define BEAT_FREQ_BAND ((float)(FREQ_BANDWIDTH) / (float)(WINDOW_SIZE + ZERO_PAD_SIZE))
#define FREQUENCY(f,s,i) ( ((f)/(s)) * i)

#endif /* _GLOBAL_H */

