
#ifndef EMAUDIO_H
#define EMAUDIO_H

#ifdef __cplusplus
extern "C" {
#endif

#include "emvector.h"

// Double buffering
typedef struct _EmAudioBufferer {
    int buffer_length;
    float *buffer1;
    float *buffer2;

    float *write_buffer;
    float *read_buffer;
    int write_offset;
} EmAudioBufferer;

void
emaudio_bufferer_reset(EmAudioBufferer *self) {
    self->write_buffer = self->buffer1;
    self->read_buffer = NULL;
    self->write_offset = 0;
}

int
emaudio_bufferer_add(EmAudioBufferer *self, int16_t s) {

    self->write_buffer[self->write_offset++] = (float)s/INT16_MAX; 

    if (self->write_offset == self->buffer_length) {
        // FIXME: error if read_buffer has not been cleared
        self->write_offset = 0;
        self->read_buffer = self->write_buffer;
        self->write_buffer = (self->read_buffer == self->buffer1) ? self->buffer2 : self->buffer1;
        return 1;
    } else {
        return 0;
    }
}

#define EMAUDIO_FFT_LENGTH 1024
#define FFT_TABLE_SIZE EMAUDIO_FFT_LENGTH/2

int
emaudio_fft(EmVector real, EmVector imag) {  

    if (real.length != EMAUDIO_FFT_LENGTH) {
        return -1;
    }
    if (imag.length != EMAUDIO_FFT_LENGTH) {
        return -2;
    }

    const bool success = emfft_transform(real.data, imag.data, EMAUDIO_FFT_LENGTH);
    if (!success) {
        return -3;
    }

    return 0;
}

// Power spectrogram
// TODO: operate in-place
int
emaudio_power_spectrogram(EmVector rfft, EmVector out, int n_fft) {
    const int spec_length = 1+n_fft/2;

    if (rfft.length < spec_length) {
        return -1;
    }
    if (out.length != spec_length) {
        return -2;
    }

    const float scale = 1.0f/n_fft;
    for (int i=0; i<spec_length; i++) {
        const float a = fabs(rfft.data[i]);
        out.data[i] = scale * powf(a, 2);
    }
    return 0;
}

// Simple formula, from Hidden Markov Toolkit
// in librosa have to use htk=True to match
float
emaudio_mels_from_hz(float hz) {
    return 2595.0 * log10(1.0 + (hz / 700.0));
}
float
emaudio_mels_to_hz(float mels) {
    return 700.0 * (powf(10.0, mels/2595.0) - 1.0);
}


typedef struct _EmAudioMel {
    int n_mels;
    float fmin;
    float fmax;
    int n_fft;
    int samplerate;
} EmAudioMel;


static int
mel_bin(EmAudioMel params, int n) {

    // Filters are spaced evenly in mel space
    const float melmin = emaudio_mels_from_hz(params.fmin);
    const float melmax = emaudio_mels_from_hz(params.fmax);
    const float melstep = (melmax-melmin)/(params.n_mels+1);

    const float mel = melmin + (n * melstep);
    const float hz = emaudio_mels_to_hz(mel);
    const int bin = floor((params.n_fft+1)*(hz/params.samplerate));
    return bin;
}


// https://haythamfayek.com/2016/04/21/speech-processing-for-machine-learning.html
int
emaudio_melspec(EmAudioMel mel, EmVector spec, EmVector mels) {

    const int max_bin = 1+mel.n_fft/2;
    if (max_bin > spec.length) {
        return -1;
    }
    if (mel.n_mels != mels.length) {
        return -2;
    }

    // Note: no normalization

    for (int m=1; m<mel.n_mels+1; m++) {
        const int left = mel_bin(mel, m-1);
        const int center = mel_bin(mel, m);
        const int right = mel_bin(mel, m+1);
    
        if (left < 0) {
            return -3;
        }
        if (right > max_bin) {
            return -4;
        } 

        float val = 0.0f;
        for (int k=left; k<center; k++) {
            const float weight = (float)(k - left)/(center - left);
            val += spec.data[k] * weight;
        }
        for (int k=center; k<right; k++) {
            const float weight = (float)(right - k)/(right - center);
            val += spec.data[k] * weight;
        }

        mels.data[m-1] = val;
    }

    return 0;
}



#define EM_RETURN_IF_ERROR(expr) \
    do { \
        const int _e = (expr); \
        if (_e != 0) { \
            return _e; \
        } \
    } while(0);

int
emaudio_melspectrogram(EmAudioMel mel_params, EmVector inout, EmVector temp) {

    const int n_fft = mel_params.n_fft;
    const int s_length = 1+n_fft/2;
    const int n_mels = mel_params.n_mels;
 
    // Apply window
    EM_RETURN_IF_ERROR(emvector_hann_apply(inout));

    // Perform (short-time) FFT
    EM_RETURN_IF_ERROR(emvector_set_value(temp, 0.0f));
    EM_RETURN_IF_ERROR(emaudio_fft(inout, temp));

    // Compute mel-spectrogram
    EM_RETURN_IF_ERROR(emaudio_power_spectrogram(inout, emvector_view(temp, 0, s_length), n_fft));
    EM_RETURN_IF_ERROR(emaudio_melspec(mel_params, temp, emvector_view(inout, 0, n_mels)));

    return 0;
}

#ifdef __cplusplus
} // extern "C"
#endif
#endif // EMAUDIO_H
