# Bird audio detection using machine learning on microcontrollers

Originally started for the DCASE2018 challenge:
[Bird audio detection](http://dcase.community/challenge2018/task-bird-audio-detection)

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

* Include RandomForestClassifier with float
* Use embedded model in train/test in notebook
* Run performance test on device
* Implement microphone input on device
* Implement sending of classification from device
* Implement sending of features,audio for subsets of samples

Improve performance in non-matched conditions

* Implement spherical K-means feature learning. Ref Stowell LifeCLEF 2014.
* Try to log-scale the spectrograms
* Try to cubic-scale the spectrograms


MLCluster

* Setup without LoadBalancer. Probably using NodePort `--set service.type=NodePort` 
* Create script for easily getting right addresses for `NodePort`

Tools

* Cluster the datasets, look at (dis)similarities of compete vs train
* Would it make sense to normalize the summarized features?
In addition to or instead of the spectrogram?
* Make tools for getting misclassified results, visualize/playback
* Visualize feature importance on top of features mean/meadian/std


