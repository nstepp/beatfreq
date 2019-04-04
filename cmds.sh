#!/bin/bash

mpg123 -s "$1" | sox -t raw -r 44100 -s -b 16 -c 2 - -t raw -r 44100 -s -b 16 -c 1 /tmp/audio_in_pipe &
play -t raw -r 44100 -e signed-integer -b 16 -c 1 /tmp/audio_out_pipe &
./beatfreq 
