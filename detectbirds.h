
#ifndef DETECTBIRDS_H
#define DETECTBIRDS_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include <stdlib.h>

#include <eml_fft.h>
#include <eml_audio.h>


// FIXME: actually include eml_trees
//#include <eml_trees.h>
typedef struct {} EmtreesModel;
int32_t
eml_trees_predict(EmlTreesModel *m, float *f, int32_t l) {
    return 0;
}
EmlTreesModel *birddetect_model = NULL;



// birddetector.h
typedef struct _BirdDetector {
    EmlVector audio;
    EmlVector features;
    EmlVector temp1;
    EmlVector temp2;
    EmlAudioMel mel_filter;
    EmlTreesModel *model;
} BirdDetector;


void
birddetector_reset(BirdDetector *self) {

    eml_vector_set_value(self->audio, 0.0f);
    eml_vector_set_value(self->features, 0.0f);
    eml_vector_set_value(self->temp1, 0.0f);
    eml_vector_set_value(self->temp2, 0.0f);
}

void
birddetector_push_frame(BirdDetector *self, EmVector frame) {

    // insert new frame into our buffer
    eml_vector_shift(self->audio, -frame.length);
    eml_vector_set(self->audio, frame, self->audio.length-frame.length);

    // process current window
    eml_vector_set(self->temp1, self->audio, 0);
    eml_audio_melspectrogram(self->mel_filter, self->temp1, self->temp2);
    eml_vector_meansub(self->temp1);

    // Feature summarization
    eml_vector_max_into(self->features, emvector_view(self->temp1, 0, self->features.length));
}

bool
birddetector_predict(BirdDetector *self) {

    const int32_t cl = eml_trees_predict(self->model, self->features.data, self->features.length);
    return cl == 1;
}

#ifdef __cplusplus
} // extern "C"
#endif

#endif // DETECTBIRDS_H

