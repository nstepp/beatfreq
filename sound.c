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

    $Id: sound.c 613 2010-08-25 17:40:47Z stepp $
*/

#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include "sound.h"
#include "global.h"

/*
 * open a named pipe from which we will read sound data
 */
int init_audio(void)
{
	int audio_pipe_fd;

	audio_pipe_fd = open(AUDIO_PIPE,O_RDONLY);

	return(audio_pipe_fd);
}

/*
 * read in a sample size worth of data
 */
int sample_audio(int audio_pipe_fd,data_t *audio_data,size_t sample_size)
{
	int i;
	short audio_sample;

	for(i=0;i<sample_size;i++) {
		if(!read(audio_pipe_fd,(void *)&audio_sample,2)) {
			return 0;
		}
		audio_data[i]=(data_t)audio_sample;
	}

	return 1;
}

/*
 * "play" audio by sending it out on a file descriptor
 */
void output_audio(int audio_out_fd, data_t *audio_data, size_t sample_size)
{
	int i;
	short audio_sample;

	for(i=0;i<sample_size;i++) {
		audio_sample = (short)audio_data[i];
		write(audio_out_fd,(void *)&audio_sample,2);
	}
}

