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

    $Id: main.c 925 2016-04-07 16:42:02Z stepp $
*/

#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <fftw3.h>
#include <ggi/ggi.h>
#include <math.h>
#include "global.h"
#include "graphics.h"
#include "window.h"
#include "math.h"
#include "sound.h"
#include "list.h"

void print_spectrum(data_t *spec, size_t n);
void fprint_spectrum(FILE *ofile, data_t *spec, size_t n);

int main()
{
	// Loop variables and file descriptors
	int i,j,k;
	int audio_pipe_fd,audio_output_fd;
	
	// data
	data_t audio_data[SAMPLE_SIZE];
	data_t avg_beat[NYQUIST_SIZE(WINDOW_SIZE)];
	data_t avg_pspec[NYQUIST_SIZE(WINDOW_SIZE)];
	data_t pspec[NYQUIST_SIZE(SAMPLE_SIZE)];
	data_t beat_fft[WINDOW_SIZE+ZERO_PAD_SIZE];
	data_t audio_fft[SAMPLE_SIZE];
	data_t zero_pad_spec[WINDOW_SIZE + ZERO_PAD_SIZE];
	
	// data windows
	window_t *sgram_window;
	window_t *pspec_window;
	window_t *beat_window;

	// visualization stuff
	visual_t *beat_visual;
	vis_scope_t *sgraph_data;

	// List that holds peak information
	maxima_list_t *peak_info;

	// FFT plans
	fftw_plan audio_fft_p;
	fftw_plan beat_fft_p;

	//visual_t *audio_visual;
	//vis_scope_t *scope_data;

	// For testing
	printf("SAMPLE_SIZE = %d\n",SAMPLE_SIZE);
	printf("WINDOW_SIZE = %d\n",WINDOW_SIZE);
	printf("SAMPLE_FREQ = %f Hz\n",(float)SAMPLE_FREQ);
	printf("FREQ_BANDWIDTH = %f Hz\n",(float)FREQ_BANDWIDTH);
	printf("SAMPLE_TIME = %f s\n",(float)SAMPLE_TIME);
	printf("NYQUIST_FREQ (sound) = %f Hz\n",(float)NYQUIST_FREQ);
	printf("WINDOW_TIME = %f s\n",(float)WINDOW_TIME);
	printf("LOW_CUTOFF = %f Hz\n",(float)LOW_CUTOFF);
	printf("BEAT_FREQ_BAND = %f Hz\n",(float)BEAT_FREQ_BAND);
	printf("BEAT_FREQ_BAND * NYQUIST_SIZE(WINDOW_SIZE) = %f Hz\n",BEAT_FREQ_BAND * NYQUIST_SIZE(WINDOW_SIZE));

	
	// Initialize things
	audio_pipe_fd     = init_audio();
	//audio_fft_p       = init_fft(SAMPLE_SIZE);
	//beat_fft_p        = init_fft(WINDOW_SIZE+ZERO_PAD_SIZE);
	audio_fft_p       = fftw_plan_r2r_1d(SAMPLE_SIZE, audio_data, audio_fft, FFTW_R2HC, 0);
	beat_fft_p        = fftw_plan_r2r_1d(WINDOW_SIZE+ZERO_PAD_SIZE, zero_pad_spec, beat_fft, FFTW_R2HC, 0);
	sgram_window      = init_window(NYQUIST_SIZE(SAMPLE_SIZE),WINDOW_SIZE);
	pspec_window      = init_window(NYQUIST_SIZE(SAMPLE_SIZE),WINDOW_SIZE);
	beat_window       = init_window(NYQUIST_SIZE(SAMPLE_SIZE),NYQUIST_SIZE(WINDOW_SIZE+ZERO_PAD_SIZE));
	beat_visual       = init_visual(WINDOW_SIZE,300);
	sgraph_data       = visualize_sgraph_init(beat_visual,NYQUIST_SIZE(WINDOW_SIZE),1.5);
	//audio_visual      = init_visual();
	//scope_data        = visualize_scope_init(audio_visual,SAMPLE_SIZE,32767);
	

	/*
	 * For testing purposes, we will write sound data out to this
	 * file descriptor.  This file must be a named pipe, and some
	 * type of audio playing program should read the data as
	 * we write it out.  To disable this, just comment out this line
	 * as well as the call to output_audio() below.
	 */
	audio_output_fd = open("/tmp/audio_out_pipe",O_WRONLY);

	// We will use this block of zeros to do zero padding.
	memset(&zero_pad_spec[WINDOW_SIZE],0,ZERO_PAD_SIZE);
	for(EVER) {
		for(i = 0; i < WINDOW_SIZE; i++) {
			// Read in an audio sample (quit when no more data)
			if(!sample_audio(audio_pipe_fd,audio_data,SAMPLE_SIZE)) {
				exit(0);
			}

			// visualize_scope_step is a visualization for actual sound data (oscilloscope)
			//visualize_scope_step(audio_visual,scope_data,audio_data,SAMPLE_SIZE);
			output_audio(audio_output_fd,audio_data,SAMPLE_SIZE);

			// Perform an FFT on the sound data
			fftw_execute(audio_fft_p);

			// Change the FFT into a power spectrum (basically complex conjugate)
			power_spectrum(pspec,audio_fft,SAMPLE_SIZE);

			// Add FFT to a data window
			// We will scroll through this
			stack_window(pspec_window,pspec);

			// Calculate secondary FFT every 32 primary FFTs
			if(i%(8) == 0) {
				
				/*
				 * pspec_window contains many FFTs from audio samples.  The
				 * window constitutes a spectorgram.  For each constant
				 * frequency band of data, compute an FFT to pick up
				 * periodic structures.
				 */

				for(j = 0; j < pspec_window->data_size; j++) {
					// Things work out better if we use a hanning window
					wapply_hanning_window(pspec_window,j);

					// Zero padding gives us some resulution needed for
					// the results to be useful
					copy_window_row(zero_pad_spec,pspec_window,j);
					fftw_execute(beat_fft_p);
					power_spectrum(&(beat_window->data[j * beat_window->window_size]),beat_fft,WINDOW_SIZE+ZERO_PAD_SIZE);

					// disregard the first 8 values, which include DC output.
					// This is hackish, and should probably be changed
					for(k=0;k<16;k++) {
						beat_window->data[j * beat_window->window_size + k]=0;
					}
				}

/*
				avg_window(pspec_window, avg_pspec);
				//wapply_hanning_window(avg_pspec,0);
				apply_hanning_window(avg_pspec, NYQUIST_SIZE(WINDOW_SIZE));
				//copy_window_row(zero_pad_spec,avg_pspec,0);
				memcpy(zero_pad_spec, avg_pspec, sizeof(data_t)*NYQUIST_SIZE(WINDOW_SIZE));
				fftw_execute(beat_fft_p);
				power_spectrum(&(beat_window->data[0]),beat_fft,WINDOW_SIZE+ZERO_PAD_SIZE);
*/
				/*
				 * Now we have a collection of our secondary FFTs.  It is
				 * here where we need to somehow combine the periodic
				 * information from all frequencies to come up with rhythmic
				 * information.  Right now, I am just taking the average of
				 * all frequencies.  This seems to work well for beats which
				 * are spread over many frequencies (like percussion),
				 * but doesn't quite cut it for something like piano music
				 * where each beat occurs at one, or a few, specific
				 * frequencies.  Averaging also cuts out noise from the
				 * FFT data.  A more complicated scheme is definitely needed
				 * here.
				 */
				avg_window(beat_window,avg_beat);
//				memcpy(avg_beat,beat_window->data,NYQUIST_SIZE(WINDOW_SIZE));
				normalize_data_1d(avg_beat,beat_window->window_size);

				// See math.c for peak finding info
				peak_info = find_peaks(avg_beat,beat_window->window_size);

				// This plots the beat FFT data.  It takes the list of peaks
				// as input so that it can print the BPMs on each peak.
				visualize_sgraph_step(beat_visual,sgraph_data,avg_beat,peak_info,beat_window->window_size);
				free_list((generic_list_t *)peak_info);
			}
		}	
	}

	// Cleanup
	//ggiClose(audio_visual->vis);
	ggiClose(beat_visual->vis);
	ggiExit();
	free_window(sgram_window);
	free_window(pspec_window);
	fftw_destroy_plan(audio_fft_p);
	fftw_destroy_plan(beat_fft_p);
	close(audio_pipe_fd);
	close(audio_output_fd);
	
	return 0;
}

void print_spectrum(data_t *spec, size_t n)
{
	size_t i;

	for(i=0;i<n;i++) {
		printf("%.5f ",spec[i]);
	}
	printf("\n");
}

void fprint_spectrum(FILE *ofile, data_t *spec, size_t n)
{
	size_t i;

	for(i=0;i<n;i++) {
		fprintf(ofile,"%d %.5f\n",i,spec[i]);
	}
	fprintf(ofile,"\n");
}


