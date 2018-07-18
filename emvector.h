
#ifndef EMVECTOR_H
#define EMVECTOR_H

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    float *data;
    ssize_t length;
} EmVector;


int
emvector_shift(EmVector a, int amount)
{
    if (abs(amount) >= a.length) {
        return -2; // non-sensical
    }

    if (amount == 0) {
        return 0;
    } else if (amount > 0) {
        return -1; // TODO: implement
    } else {
        for (int i=a.length+amount; i<a.length; i++) {  
            a.data[i+amount] = a.data[i];
        }
        return 0;
    }
}

int
emvector_set(EmVector dest, EmVector source, int location) {
    const int final_dest = source.length+location;
    if (final_dest > dest.length) {
        return -1;
    }
    if (location < 0) {
        return -2;
    }

    for (int i=location; i<final_dest; i++) {
        dest.data[i] = source.data[i-location]; 
    }
    return 0;
}

#define EM_MAX(a, b) (a > b) ? a : b

int
emvector_max_into(EmVector a, EmVector b) {
    if (a.length != b.length) {
        return -1;
    }

    for (ssize_t i=0; i<a.length; i++) {
        a.data[i] = EM_MAX(a.data[i], b.data[i]);
    }
    return 0;
}

int
emvector_set_value(EmVector a, float val) {
    for (ssize_t i=0; i<a.length; i++) {
        a.data[i] = val;
    }
    return 0;
}

float
emvector_mean(EmVector v) {
    float sum = 0.0f;
    for (ssize_t i=0; i<v.length; i++) {
        sum += v.data[i];
    }
    float mean = sum/v.length; 
    return mean;
}

int
emvector_subtract_value(EmVector v, float val) {

    for (ssize_t i=0; i<v.length; i++) {
        v.data[i] -= val;
    }
    return 0;
}

EmVector
emvector_view(EmVector orig, int start, int end) {
    const int length = end-start;
    return (EmVector){ orig.data+start, length };
}

// Mean subtract normalization
int
emvector_meansub(EmVector inout) {

    const float mean = emvector_mean(inout);
    emvector_subtract_value(inout, mean);

    return 0;
}


// Hann window
int
emvector_hann_apply(EmVector out) {

    const long len = out.length;
    for (int i=0; i<len; i++) {
        float m = 0.5 * (1 - cos(2*M_PI*i/(len-1)));
        out.data[i] = m * out.data[i];
    }
    return 0;
}

#ifdef __cplusplus
} // extern "C"
#endif
#endif // EMVECTOR_H
