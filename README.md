# Bird audio detection using machine learning on microcontrollers

Originally started for the DCASE2018 challenge: [Bird audio detection](http://dcase.community/challenge2018/task-bird-audio-detection)

## Goals

* Machine-learning classifier running in real-time on low-power microcontroller
for use in a Wireless Sensor Network doing continious acoustic monitoring
* Classification accuracy good enough for real-time pre-processing,
relative abundance reporting. AUC ROC 80%+ under mismatched conditions.
* Validate approach by collecting data using installed units

## Status

* Good performance on testset, **bad generalization to compete set**
* Python model setup with sklearn
* C model mostly complete, only tested on Linux

## TODO

Run on hardware

* Include emtrees.RandomForestClassifier with float
* Use embedded model in train/test in notebook
* Run performance test on ESP32
* Implement microphone input on ESP32

DCASE2018 submission

* Fix predictions not outputting probability as floating-point

Improve performance in non-matched conditions

* Implement OSKmeans feature learning. Ref Stowell LifeCLEF 2014
* Try some data augmentation to make robust against variations and noise
* Try a pre-emphasis filter
* Try to add a highpass filter to reduce noise
* Try to add a lowpass filter to reduce noise
* Try to log-scale the spectrograms
* Try denoising the audio
* Try an SNR estimator as feature

Tools

* Cluster the datasets, look at (dis)similarities of compete vs train
* Would it make sense to normalize the summarized features?
In addition to or instead of the spectrogram?
* Make tools for getting misclassified results, visualize/playback
* Visualize feature importance on top of features mean/meadian/std

C library

* Implement mean/stddev summarization (streamed)
* Implement histogram statistics. skewness,kurtosis
* Implement low/highpass filters
* Implement preemphasis filter
* Implement more features. RMS, zero-crossings, spectral centroid, spectral flatness
* Implement MFCC calculation (DCT)

Other interesting approaches

* Try HOG dictionary feature learning
* Deep learning based, adapter for microcontrollers. CNN/CRNN
* OpenSMILE feature learning, segment probabilities. Mario



