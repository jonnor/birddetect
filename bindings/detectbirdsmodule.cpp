
#include <stdio.h>

#include <vector>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/numpy.h>

#include "../detectbirds.h"

namespace py = pybind11;


// FIXME: unhardcode
#define AUDIO_HOP_LENGTH 512
#define AUDIO_WINDOW_LENGTH 1024

class Processor {
private:
    BirdDetector detector;
    EmlAudioBufferer bufferer;

    float record1[AUDIO_HOP_LENGTH];
    float record2[AUDIO_HOP_LENGTH];
    float input_data[AUDIO_WINDOW_LENGTH];
    float temp1_data[AUDIO_WINDOW_LENGTH];
    float temp2_data[AUDIO_WINDOW_LENGTH];

public:
    Processor(int n_mels, float fmin, float fmax, int n_fft, int samplerate) {

        bufferer = (EmlAudioBufferer){ AUDIO_HOP_LENGTH, record1, record2, NULL, NULL, 0 };

        const EmlAudioMel params = {
            n_mels: n_mels,
            fmin: fmin,
            fmax: fmax,
            n_fft:AUDIO_WINDOW_LENGTH,
            samplerate:samplerate,
        };

        const int features_length = params.n_mels; // only 1 feature per mel band right now
        float features_data[features_length];

        detector = {
            audio: (EmlVector){ input_data, AUDIO_WINDOW_LENGTH },
            features: (EmlVector){ features_data, features_length },
            temp1: (EmlVector){ temp1_data, AUDIO_WINDOW_LENGTH },
            temp2: (EmlVector){ temp2_data, AUDIO_WINDOW_LENGTH },
            mel_filter: params,
            model: birddetect_model,
        };

        reset();
    }

    void reset() {
        birddetector_reset(&detector);
        eml_audio_bufferer_reset(&bufferer);
    }

    int add_samples(py::array_t<float, py::array::c_style | py::array::forcecast> in) {
        if (in.ndim() != 1) {
            throw std::runtime_error("process() input must have dimensions 1");
        }

        float *samples = (float *)in.data();
        int length = in.shape(0);

        int n_frames = 0;
        for (int i=0; i<length; i++) {
            eml_audio_bufferer_add(&bufferer, samples[i]);

            if (bufferer.read_buffer) {
                EmlVector frame = { bufferer.read_buffer, bufferer.buffer_length };
                birddetector_push_frame(&detector, frame);
                bufferer.read_buffer = NULL; // done processing

                n_frames += 1;
            }

        }
        return n_frames;
    }

    py::array_t<float> get_features() {

        const int length = detector.features.length;
        auto ret = py::array_t<float>(length);
        EmlVector out = { (float *)ret.data(), length };
        eml_vector_set(out, detector.features, 0);

        return ret;
    }

    bool predict() {
        return birddetector_predict(&detector);
    }

};


PYBIND11_MODULE(detectbirds, m) {
    m.doc() = "Detect birdsong in audio";

    py::class_<Processor>(m, "Processor")
        .def(py::init< int, float, float, int, int >())
        .def("add_samples", &Processor::add_samples)
        .def("predict", &Processor::predict)
        .def("get_features", &Processor::get_features)
        .def("reset", &Processor::reset);

}

