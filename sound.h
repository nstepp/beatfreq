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

    $Id: sound.h 613 2010-08-25 17:40:47Z stepp $
*/

#ifndef _SOUND_H
#define _SOUND_H

#include "global.h"

#define AUDIO_PIPE "/tmp/audio_in_pipe"

int init_audio(void);
int sample_audio(int audio_pipe_fd,data_t *audio_data,size_t sample_size);
void output_audio(int audio_out_fd, data_t *audio_data, size_t sample_size);

#endif /* _SOUND_H */
