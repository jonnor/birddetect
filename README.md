# DCASE2018 bird detection

DCASE2018 challenge: [Bird audio detection](http://dcase.community/challenge2018/task-bird-audio-detection)

## Goals

* Classifier running on low-power 32-bit microcontroller,
with high enough classification rate to be useful as recording trigger.
Should be able to run online classification in real-time.
* Submit for DCASE18 challenge. July 31. Classification, code, technical paper. 

## Status

* Good performance on testset, **bad generalization to compete set**
* Python model setup with sklearn
* C model mostly complete, only tested on Linux

## TODO

Run on hardware

* Include emtrees.RandomForestClassifier with float
* Use embedded model in train/test in notebook
* Run performance test on ESP32

DCASE2018 submission

* Setup Latex build
* Write outline
* Create diagrams
* Finish report

Improve model generalization on compete set

* Use full birdvox dataset
* Move minmax scaling on frame, do it on summarized instead
* Add min,mean,std summarization
* Add some basic features on frames. RMS, zero-crossings, spectral centroid, spectral flatness
* Add features on summarized spectrogram. mean,median,mode,skewness,kurtosis
* Try a pre-emphasis filter
* Try to add a highpass filter to reduce noise
* Try to add a lowpass filter to reduce noise
* Try longer frame size
* Cluster the datasets, look at (dis)similarities of compete vs train
* Would it make sense to normalize the summarized features?
In addition to or instead of the spectrogram?
* Make tools for getting misclassified results, visualize/playback
* Visualize feature importance on top of feature mean/meadian/std
* Try some data augmentation to make robust against variations
* Try an SNR estimator as feature
* Try denoising
* Augment labeled data by adding noise
* Try to log-scale the spectrograms

Later

* Try OSKmeans feature learning. Ref Stowell LifeCLEF 2014
* Try HOG dictionary feature learning.
* Add tests/benchmark for Goertzel, see if possible to optimize.
Can one use it to only extract bands shown to be important per feature importance (estimated by RF)



