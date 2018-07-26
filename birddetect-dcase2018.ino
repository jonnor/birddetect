
#include "detectbirds.h"

#define AUDIO_HOP_LENGTH 512
#define AUDIO_WINDOW_LENGTH 1024

class Processor {
    BirdDetector detector;
    EmAudioBufferer bufferer;

    float record1[AUDIO_HOP_LENGTH];
    float record2[AUDIO_HOP_LENGTH];
    float input_data[AUDIO_WINDOW_LENGTH];
    float temp1_data[AUDIO_WINDOW_LENGTH];
    float temp2_data[AUDIO_WINDOW_LENGTH];

public:
    Processor(int n_mels, float fmin, float fmax, int n_fft, int samplerate) {

        bufferer = (EmAudioBufferer){ AUDIO_HOP_LENGTH, record1, record2, NULL, NULL, 0 };

        const EmAudioMel params = {
            n_mels: n_mels,
            fmin: fmin,
            fmax: fmax,
            n_fft: AUDIO_WINDOW_LENGTH,
            samplerate:samplerate,
        };

        const int features_length = params.n_mels; // only 1 feature per mel band right now
        float features_data[features_length];

        detector = {
            audio: (EmVector){ input_data, AUDIO_WINDOW_LENGTH },
            features: (EmVector){ features_data, features_length },
            temp1: (EmVector){ temp1_data, AUDIO_WINDOW_LENGTH },
            temp2: (EmVector){ temp2_data, AUDIO_WINDOW_LENGTH },
            mel_filter: params,
            model: birddetect_model,
        };

        reset();
    }

    void reset() {
        birddetector_reset(&detector);
        emaudio_bufferer_reset(&bufferer);
    }

    bool add_sample(float sample) {

        emaudio_bufferer_add(&bufferer, sample);

        if (bufferer.read_buffer) {
            EmVector frame = { bufferer.read_buffer, bufferer.buffer_length };
            birddetector_push_frame(&detector, frame);
            bufferer.read_buffer = NULL; // done processing
        }
        return true;
    }

    bool predict() {
        return birddetector_predict(&detector);
    }
};

Processor processor(64, 500, 15000, 1024, 44100);

#if 0
#include <driver/i2s.h>

const i2s_port_t I2S_PORT = I2S_NUM_0;
const int BLOCK_SIZE = 1024;

void setupMicrophone() {
  esp_err_t err;

  // The I2S config as per the example
  const i2s_config_t i2s_config = {
      .mode = i2s_mode_t(I2S_MODE_MASTER | I2S_MODE_RX), // Receive, not transfer
      .sample_rate = 16000,                         // 16KHz
      .bits_per_sample = I2S_BITS_PER_SAMPLE_32BIT, // could only get it to work with 32bits
      .channel_format = I2S_CHANNEL_FMT_ONLY_RIGHT, // although the SEL config should be left, it seems to transmit on right
      .communication_format = i2s_comm_format_t(I2S_COMM_FORMAT_I2S | I2S_COMM_FORMAT_I2S_MSB),
      .intr_alloc_flags = ESP_INTR_FLAG_LEVEL1,     // Interrupt level 1
      .dma_buf_count = 8,                           // number of buffers
      .dma_buf_len = BLOCK_SIZE                     // samples per buffer
  };

  // The pin config as per the setup
  const i2s_pin_config_t pin_config = {
      .bck_io_num = 14,   // BCKL
      .ws_io_num = 15,    // LRCL
      .data_out_num = -1, // not used (only for speakers)
      .data_in_num = 32   // DOUT
  };

  // Configuring the I2S driver and pins.
  // This function must be called before any I2S driver read/write operations.
  err = i2s_driver_install(I2S_PORT, &i2s_config, 0, NULL);
  if (err != ESP_OK) {
    Serial.printf("Failed installing driver: %d\n", err);
  }
  err = i2s_set_pin(I2S_PORT, &pin_config);
  if (err != ESP_OK) {
    Serial.printf("Failed setting pin: %d\n", err);
  }

}

void readMicrophone() {
  int32_t samples[BLOCK_SIZE];
  const int timeout = 10; // in RTOS ticks
  const int num_bytes_read = i2s_read_bytes(I2S_PORT, (char *)samples, 
                                      BLOCK_SIZE,  // the doc says bytes, but it is elements??
                                      timeout);
  
  // FIXME: how does this make sense
  const int samples_read = num_bytes_read / 8;
  if (samples_read > 0) {

    float mean = 0;
    for (int i = 0; i < samples_read; ++i) {
      mean += samples[i];
    }

    for (int i = 0; i < samples_read; ++i) {
      processor.add_sample(samples[i]);
    }

    Serial.print("mean: ");
    Serial.println(mean);
  }
}

#endif

void setup() {
  Serial.begin(115200);
//  Serial.println("Configuring microphone...");
//  setupMicrophone();
//  Serial.println("I2S driver installed.");
}

void loop() {


}


