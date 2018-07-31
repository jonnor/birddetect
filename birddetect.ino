struct Config {
  const String prefix = "";
  const String role = "birddetect/1";

  const char *wifiSsid = "jonnor24";
  const char *wifiPassword = "";

  const char *mqttHost = "192.168.0.101";
  const int mqttPort = 1883;

  const char *mqttUsername = NULL;
  const char *mqttPassword = NULL;
} cfg;

#include <WiFi.h>
#include <WiFiClient.h>
#include <WiFiClientSecure.h>

#include <PubSubClient.h>
#include <Msgflo.h>

WiFiClient wifiClient;
PubSubClient mqttClient;
msgflo::Engine *engine;
  String clientId = cfg.role + WiFi.macAddress();

auto participant = msgflo::Participant("birddetect/Processor", cfg.role);
msgflo::OutPort *outPort = nullptr;

#include "detectbirds.h"

#define AUDIO_HOP_LENGTH 512
#define AUDIO_WINDOW_LENGTH 1024

class Processor {
public:
    BirdDetector detector;
    EmAudioBufferer bufferer;

    float record1[AUDIO_HOP_LENGTH];
    float record2[AUDIO_HOP_LENGTH];
    float input_data[AUDIO_WINDOW_LENGTH];
    float temp1_data[AUDIO_WINDOW_LENGTH];
    float temp2_data[AUDIO_WINDOW_LENGTH];

    int n_frames;

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
        n_frames = 0;
        birddetector_reset(&detector);
        emaudio_bufferer_reset(&bufferer);
    }

    bool add_sample(float sample) {

        emaudio_bufferer_add(&bufferer, sample);

        if (bufferer.read_buffer) {
            EmVector frame = { bufferer.read_buffer, bufferer.buffer_length };
            birddetector_push_frame(&detector, frame);
            bufferer.read_buffer = NULL; // done processing
            n_frames += 1;
        }
        return true;
    }

    bool predict() {
        return birddetector_predict(&detector);
    }
};

Processor processor(64, 500, 15000, 1024, 44100);
long frames_processing_time = 0;


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

int parseFloatArray(const char *buf, float *values, size_t values_length) {
  char *err;
  const char *p = buf;
  int i = 0;

  while (*p) {
    const double val = strtod(p, &err);
    if (i == values_length) {
      return -1;
    }
    if (p == err) {
      p++;
    } else if ((err == NULL) || (*err == 0)) { 
      values[i++] = val;
      break;
    } else {
      values[i++] = val;
      p = err + 1;
    }
  }
  return i;
}



void setupMsgflo() {

  participant.icon = "toggle-on";

  mqttClient.setServer(cfg.mqttHost, cfg.mqttPort);
  mqttClient.setClient(wifiClient);

  engine = msgflo::pubsub::createPubSubClientEngine(participant, &mqttClient, clientId.c_str(), cfg.mqttUsername, cfg.mqttPassword);

  outPort = engine->addOutPort("prediction", "any", cfg.prefix+cfg.role+"/prediction");

  msgflo::InPort *framePort = engine->addInPort("frame", "boolean", cfg.prefix+cfg.role+"/frame",
  [](byte *data, int length) -> void {

    long frame_time = 0;
    size_t VALUES_MAX = 512;
    float values[VALUES_MAX];
    std::string str((char *)data, length);
    const int n_values = parseFloatArray(str.c_str(), values, VALUES_MAX);

    Serial.print("got values: ");
    Serial.println(n_values);
    
    const long before = micros();
    for (int i=0; i<n_values; i++){ 
      processor.add_sample(values[i]);
    }
    frames_processing_time += (micros() - before);
  });
  
  msgflo::InPort *predictPort = engine->addInPort("predict", "boolean", cfg.prefix+cfg.role+"/predict",
  [](byte *data, int length) -> void {

    const long before = micros();
    const int hasbird = processor.predict() ? 1 : 0;
    const long predict_time = (micros() - before);

    const size_t OUT_MAX = 1024;
    char out[OUT_MAX];
    
    snprintf(out, OUT_MAX, "%d;%d;%ld;%ld\n",
      hasbird, processor.n_frames, frames_processing_time, predict_time);
    outPort->send(out);
       
    frames_processing_time = 0;
  });
  
}



void setup() {
  Serial.begin(115200);
  delay(100);
  Serial.println();
  Serial.println();
  Serial.println();

//  Serial.println("Configuring microphone...");
//  setupMicrophone();
//  Serial.println("I2S driver installed.");


  WiFi.mode(WIFI_STA);
  Serial.printf("Configuring wifi: %s\r\n", cfg.wifiSsid);
  WiFi.begin(cfg.wifiSsid, cfg.wifiPassword);

  setupMsgflo();
}

void loop() {
  static bool connected = false;

  if (WiFi.status() == WL_CONNECTED) {
    if (!connected) {
      Serial.printf("Wifi connected: ip=%s\r\n", WiFi.localIP().toString().c_str());
    }
    connected = true;
    engine->loop();
  } else {
    if (connected) {
      connected = false;
      Serial.println("Lost wifi connection.");
    }
  }
}



