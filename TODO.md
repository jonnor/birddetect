## TODO

Run on hardware

* Include RandomForestClassifier with float
* Use embedded model in train/test in notebook
* Run performance test on device
* Implement microphone input on device
* Implement sending of classification from device
* Implement sending of features,audio for subsets of samples

Improve performance in non-matched conditions

* Estimate non-matched conditions by using folds by dataset
* Implement spherical K-means feature learning. Ref Stowell LifeCLEF 2014.

MLCluster

* Setup without LoadBalancer. Probably using NodePort `--set service.type=NodePort` 
* Create script for easily getting right addresses for `NodePort`
