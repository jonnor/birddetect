# DCASE2018 bird detection

DCASE2018 challenge: [Bird audio detection](http://dcase.community/challenge2018/task-bird-audio-detection)

## Goals

* Classifier running on low-power ARM Cortex M4F,
with high enough classification rate to be useful as recording trigger.
Should be able to run as online classification in real-time.
* Submit for DCASE18 challenge. July 31. Classification, code, technical paper. 

## Status

* Good performance on testset, **bad generalization to compete set**
* Python model setup with sklearn
* C model mostly complete, only tested on Linux

## TODO

Improve model generalization on compete set

* Increase birdvox dataset to 5k sampled
* Look at PCAed features, per-class histograms
* Try to add a highpass filter to reduce noise
* Try to add a lowpass filter to reduce noise
* Try to add more features. mean,std,min ?
* Try longer frame size, or median filtering the spectrogram
* Cluster the datasets, look at (dis)similarities of compete vs train
* Would it make sense to normalize the summarized features?
In addition to or instead of the spectrogram?
* Setup 3-way cross-validation
* Make tools for getting misclassified results, visualize/playback
* Visualize feature importance on top of feature mean/meadian/std
* Try some data augmentation to make robust against variations
* Try an SNR estimator as feature
* Try denoising
* Augment data by adding noise
* Try a pre-emphasis filter?
* Try to log-scale the spectrograms

Cleanup

* Move exploratory parts to dedicated notebook
* Explain basics of challenge, generalization
* Explain classifier approach in notebook
* Make precomputed features available

Run on hardware

* Verify C feature processing
* Run C code classifier on Py features
* Setup Nordic NRF52 dev environment
* Try to run emtrees.RandomForestClassifier on M4F. Precalculated melspec-maxp features
* Try to run feature calculation on M4F

Later

* Try OSKmeans feature learning. Ref Stowell LifeCLEF 2014
* Try HOG dictionary feature learning.
* Add tests/benchmark for Goertzel, see if possible to optimize.
Can one use it to only extract bands shown to be important per feature importance (estimated by RF)



