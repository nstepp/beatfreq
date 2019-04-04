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

    $Id: test.c 613 2010-08-25 17:40:47Z stepp $
*/

#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <rfftw.h>

#define EVER ;;
#define SAMPLE_SIZE 512
#define WINDOW_SIZE 1024
#define WINDOW_LENGTH 1024

void normalize_spectrum(fftw_real *spec, size_t n);
void print_spectrum(fftw_real *spec, size_t n);
void flip_spec_window(fftw_real **pspec, fftw_real **fspec, size_t m, size_t n);
		
int main()
{
	int audio_pipe_fd,i,j,k;
	fftw_real input_data[SAMPLE_SIZE];
	fftw_real output_window[WINDOW_SIZE][SAMPLE_SIZE];
	fftw_real power_spectrum[WINDOW_SIZE][SAMPLE_SIZE/2 + 1];
	rfftw_plan fft_plan;
	rfftw_plan fft_flip_plan;

	printf("Creating FFTW Plan...");fflush(stdout);
	fft_plan = rfftw_create_plan(SAMPLE_SIZE, FFTW_REAL_TO_COMPLEX, FFTW_ESTIMATE);
	printf(".Done.\n");
	
	printf("Creating Flip FFTW Plan...");fflush(stdout);
	fft_flip_plan = rfftw_create_plan(WINDOW_SIZE, FFTW_REAL_TO_COMPLEX, FFTW_ESTIMATE);
	printf(".Done.\n");
	
	printf("Opening audio pipe....\n");
	audio_pipe_fd = open("/tmp/audio_pipe",O_RDONLY);

	printf("Beginning sample....\n");
	for(i=0;i<WINDOW_SIZE;i++) {
		printf("Sample %d",i);fflush(stdout);
		read(audio_pipe_fd,(void *)input_data,sizeof(input_data));
		
		/* fft into output_window[i] */
		printf("! - FFT");fflush(stdout);
		rfftw_one(fft_plan, input_data, output_window[i]);
		printf("! - SPEC");fflush(stdout);
		
		power_spectrum[i][0] = output_window[i][0]*output_window[i][0]; /*DC*/
		for(k=1;k<(SAMPLE_SIZE+1)/2;++k) {
			power_spectrum[i][k] = output_window[i][k]*output_window[i][k] + output_window[i][SAMPLE_SIZE-k]*output_window[i][SAMPLE_SIZE-k];
		}
		power_spectrum[i][SAMPLE_SIZE/2] = output_window[i][SAMPLE_SIZE/2]*output_window[i][SAMPLE_SIZE/2];  /* Nyquist freq. */
		printf("! NF=%f\n",power_spectrum[i][SAMPLE_SIZE/2]);

		printf("Normalizing Spectrum....\n");
		normalize_spectrum(power_spectrum[i],SAMPLE_SIZE/2);
		printf("NS: ");
		print_spectrum(power_spectrum[i],SAMPLE_SIZE/2);
	}
	printf("Done sampling.\n");
	printf("Flipping spectrum for second FFT\n");
	//flip_spec_window(power_spectrum,flip_spec,WINDOW_SIZE,SAMPLE_SIZE/2+1);
	rfftw_destroy_plan(fft_plan);
	close(audio_pipe_fd);
}

void flip_spec_window(fftw_real **pspec, fftw_real **fspec, size_t m, size_t n)
{
	int i,j;

	for(i=0;i<m;i++) {
		for(j=0;j<n;j++) {
			fspec[j][i]=pspec[i][j];
		}
	}
}

void normalize_spectrum(fftw_real *spec, size_t n)
{
	size_t i;
	fftw_real max=0.0;

	/* find max */
	for(i=0;i<n;i++) {
		if(spec[i]>max) {
			max=spec[i];
		}
	}

	/* normalize by max */
	for(i=0;i<n;i++) {
		spec[i]=spec[i]/max;
	}
}

void print_spectrum(fftw_real *spec, size_t n)
{
	size_t i;

	for(i=0;i<n;i++) {
		printf("%.5f ",spec[i]);
	}
	printf("\n");
}

