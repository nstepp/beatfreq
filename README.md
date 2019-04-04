# beatfreq

Beatfreq first creates a spectrogram of input audio, then performs another Fourier transform on the spectrogram to determine rhythmic structure. The resulting peaks show separate BPM components at their relative intensities.

This package is rather old, and there several updates required to bring it into modern times. The two big updates required are:
- The visualization routines use GGI, which should probably be replaced by a more actively maintained library
- Audio input and output uses pipes. This should be replaced with proper communication with underlying sound devices and mp3 or wav files.

