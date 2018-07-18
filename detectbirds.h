
#ifndef DETECTBIRDS_H
#define DETECTBIRDS_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include <stdlib.h>

#include "emfft.h"
#include "emaudio.h"


// FIXME: actually include emtrees
//#include <emtrees.h>
typedef struct {} EmtreesModel;
int32_t
emtrees_predict(EmtreesModel *m, float *f, int32_t l) {
    return 0;
}
EmtreesModel *birddetect_model = NULL;



// birddetector.h
typedef struct _BirdDetector {
    EmVector audio;
    EmVector features;
    EmVector temp1;
    EmVector temp2;
    EmAudioMel mel_filter;
    EmtreesModel *model;
} BirdDetector;


void
birddetector_reset(BirdDetector *self) {

    emvector_set_value(self->audio, 0.0f);
    emvector_set_value(self->features, 0.0f);
    emvector_set_value(self->temp1, 0.0f);
    emvector_set_value(self->temp2, 0.0f);
}

void
birddetector_push_frame(BirdDetector *self, EmVector frame) {

    // insert new frame into our buffer
    emvector_shift(self->audio, -frame.length);
    emvector_set(self->audio, frame, self->audio.length-frame.length);

    // process current window
    emvector_set(self->temp1, self->audio, 0);
    emaudio_melspectrogram(self->mel_filter, self->temp1, self->temp2);
    emvector_meansub(self->temp1);

    // Feature summarization
    emvector_max_into(self->features, emvector_view(self->temp1, 0, self->features.length));
}

bool
birddetector_predict(BirdDetector *self) {

    const int32_t cl = emtrees_predict(self->model, self->features.data, self->features.length);
    return cl == 1;
}

#ifdef __cplusplus
} // extern "C"
#endif

#endif // DETECTBIRDS_H

