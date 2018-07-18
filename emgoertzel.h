
#ifndef EMGOERTZEL_H
#define EMGOERTZEL_H

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _EmGoertzel {
    float sine;
    float cosine;
} EmGoertzel;

EmGoertzel
emgoertzel_coefficients(int frequency, int n_samples, int sr)
{
    const float samples = (float)n_samples;
    const float k = (int)(0.5 + ((samples * frequency) / sr));
    const float omega = (2.0 * M_PI * k) / samples;
    const EmGoertzel c  = { sin(omega), cos(omega) }; 
    return c;
}


float
emgoertzel_run(EmGoertzel c, const float* data, int n_samples)
{
    const float coeff = 2.0 * c.cosine;
    float q0=0;
    float q1=0;
    float q2=0;

    for(int i=0; i<n_samples; i++) {
        q0 = coeff * q1 - q2 + data[i];
        q2 = q1;
        q1 = q0;
    }
 
    const float scale = n_samples / 2.0;
    const float real = (q1 - q2 * c.cosine) / scale;
    const float imag = (q2 * c.sine) / scale;

    const float magnitude = sqrtf(real*real + imag*imag);
    return magnitude;
}

#ifdef __cplusplus
} // extern "C"
#endif

#endif // EMGOERTZEL_H
