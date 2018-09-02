
## Other datasets
http://www.kaggle.com/c/the-icml-2013-bird-challenge
https://www.kaggle.com/c/mlsp-2013-birds/
https://www.xeno-canto.org/

## Literature review
Specific to bird audio detection.

[Automatic acoustic detection of birds through deep learning: the first Bird Audio Detection challenge](https://arxiv.org/abs/1807.05812).
D.Stowell, 2018. Explaining the generalization challenge, performance on mismatched data.
State-of-the-art CNN/CRNN methods achieve AUC ROC 88% on mismatched set.
High perf baseline with RandomForest with OSKmeans feature learning performed 79% on mismatched set.

[Bird Audio Detection: tips on building robust detectors](http://machine-listening.eecs.qmul.ac.uk/2016/11/bird-audio-detection-tips-on-building-robust-detectors/)
Filtering.
Noise reduction.
Data normalization. Amplitude normalized. Spectral whitening. Linear predictive coding filtering.
Representation. Default: Spectrogram, MFCC. Not getting good improvements by changing?
Data augmentation.
Self-adaptation.
Regularisation.
Combining models.

[Bird Audio Detection: baseline tests – and the problem of generalisation](http://machine-listening.eecs.qmul.ac.uk/2016/10/bird-audio-detection-baseline-generalisation/). First baseline of MFCCs and GMM method generalizes very badly to samples from unseen training sets.
Second baseline of spherical k-means feature learning, followed by a Random Forest classifier still managed 80% on unseen training sets.


[Bird detection in audio: a survey and a challenge](https://arxiv.org/abs/1608.03417). D.Stowell, 2016. Introducing DCASE2016.
Usecases: Unattended monitoring, prefiltering step before other automatic analyses such as bird species classification.
Detection types:
Presence/absence in a given sound clip: a detector outputs a zero if none of the target species are detected, and a one otherwise.
Monotonic segmentation. Partition the time axis into positive and negative regions. Analogous to voice activity detection (VAD).
Methods:
Energy thresholding.
Spectrogram cross-correlation.
Hidden Markov Models.
Template matching with Dynamic Time Warping.
Open question whether the various different approaches (for single species detection)
can simply be aggregated under a meta-algorithm to produce species-agnostic output.
Sinousoidal tracks.

[Detection and Classification of Acoustic Scenes and Events: Outcome of the DCASE 2016 Challenge](https://ieeexplore.ieee.org/document/8123864/).
November 2017.

2018 BirdCLEF Baseline System. [Paper](https://arxiv.org/pdf/1804.07177.pdf).
[Github](https://github.com/kahst/BirdCLEF-Baseline)
Feature extraction: https://github.com/kahst/BirdCLEF-Baseline/blob/master/utils/audio.py#L115
Use a high-pass and a low-pass filter with cut-off frequencies of 300 Hz and 15 kHz
Uses a simple SNR estimation to not train on samples with bad signal2noise ratio.
Based on median filtering and morphological operations on spectogram.
Very low score => unlikely to contain any birds.


[Automatic large-scale classification of bird sounds is strongly improved by unsupervised feature learning](https://peerj.com/articles/488/).
D. Stowell, 2014. Classifier got strongest audio-only results in LifeCLEF2014.
Inspired by techniques that have proven useful in other domains.
Compare twelve different feature representations derived from the Mel spectrum, using four large and diverse databases of bird vocalisations. Classified using a random forest classifier.
"in our classification tasks, MFCCs can often lead to worse performance than the raw Mel spectral data from which they are derived"
"unsupervised feature learning provides a substantial boost over MFCCs and Mel spectra without adding computational complexity after the model has been trained"
Using spherical k-means, adapted to run in streaming fashion using online Hartigan k-means. Using two passes, first with reservoir subsampling.
Birdsong often contains rapid temporal modulations, and this information should be useful for identifying species-specific characteristics.
feature learning is that it can be applied not only to single spectral frames, but to short sequences (or “patches”) of a few frames.
Also tested a two-layer version, second layer downsampled projected data by 8 then applying feature learning again. 
.. Spherical k-means implementations: [spherecluster](https://github.com/clara-labs/spherecluster), [OSKMeans](https://dfzljdn9uc3pi.cloudfront.net/2014/488/1/oskmeans.py.txt).
Note: Learns fully-vertical frames. 
* What if instead one learns smaller patches of say 5x5 (CNN style)?
Applied multiple times vertically within frame to get new feature vectors.
Can maybe also apply it multiple times horizontally inside a frame, and then max-pool.
Can probably analyze such patches without needing streaming or minibatch?
With 10k 10s clips, 100 random patches per clip, still under 100MB.
Would there be a benefit to learn features only/primarily from positive samples? Or atleast eliminate those that are very noisy..
Feature size gets pretty big pretty fast. (n_mels-kern_size)x(n_bases), ex 59x100...
But maybe n_bases down to 10 could work. bubul used 16 maps,1 sparrow 32 maps
Can one use feature-importance from RandomForest to drop not-needed features in dictionary?
Depending on what we find, could be that one should run k-means only on some subset of frequency bins. Or split feature learning into multiple independent areas.
Ref Learning Feature Representations with k-means, Coates/Ng.

[Two Convolutional Neural Networks for Bird Detection in Audio Signals](http://www.ofai.at/~jan.schlueter/pubs/2017_eusipco.pdf).
Describes a global CNN `bubul` (winner DCASE2017, reference DCASE2018)  and a local CNN `sparrow` with nearly equal performance
of 88.x% AUC ROC. Several other contestants also had  88.x% results.
Both have 300k tunable parameters. `sparrow` receptive field was 1.5seconds, 103 frames @ 22kHz.

[Unsupervised dictionary extraction of bird vocalisations and new tools on assessing and visualising bird activity](https://www.sciencedirect.com/science/article/pii/S1574954115000102?via%3Dihub). I.Potamitis, March 2015, Ecological Informatics.
Tool 1) Report if a recording is void or not of any birds' vocalisation activity (binary classification).
Shows 3 related methods based on image-processing of the spectrogram to create a codebook with regions-of-interests.
Regions-of-interests are then cross-correlated with samples. ROI aka spectral templates, spectral blobs, acoustic atoms.


[Classification of Bird Sounds Using Codebook Features](https://link.springer.com/chapter/10.1007/978-3-319-75417-8_21). Alfonso B. Labao, Feb 2018, ACIIDS 2018.
The codebook approach on MFCC features with a Random Forest classifier performs best with an accuracy of 93.62%.
100 to 500 codebook clusters are formed from raw features, a “one-step” approach.
Compared features, increasing complexity.
1. Spectral center and bandwidth. 
2. Histogram of spectral center and bandwidth. Frequency 100 bins, bandwidths 50 bins. N=5000. Normalize to a PDF.
3. Codebook of spectral densities. k-means clustering, 100-500. Count number of frames that hit. Normalize to a PDF.
4. Codebook of Mel frequencies
5. Summarized MFCC coefficients
6. Codebook of MFCC coefficients

[Robust feature representation for classification of bird song syllables](https://asp-eurasipjournals.springeropen.com/articles/10.1186/s13634-016-0365-8). Maria Sandsten, EURASIP Journal on Advances in Signal Processing, 2016.
A novel feature set for low-dimensional signal representation, designed for classification or clustering of non-stationary signals with complex variation in time and frequency. Applied to birdsong and *within-species* classification.
Ambiguity spectrum. Multitapers. Singular Value Decomposition.


[An FPGA-Based WASN for Remote Real-Time Monitoring of Endangered Species: A Case Study on the Birdsong Recognition of Botaurus stellaris](http://www.mdpi.com/1424-8220/17/6/1331). Wireless Acoustic Sensor Networks (WASN). #TODO


[Design and Implementation of a Robust Acoustic Recognition System for Waterbird Species using TMS320C6713 DSK](https://www.igi-global.com/gateway/article/176715). A. Boulmaiz. International Journal of Ambient Computing and Intelligence (IJACI), 2017. 
Tonal region detector (TRD) using a sigmoid function.
Mel Frequency Cepstral Coefficients, Spectral Subtraction. Support Vector Machine. #TODO

[Audio Classification of Bird Species: A Statistical Manifold Approach](https://www.researchgate.net/publication/220765656_Audio_Classification_of_Bird_Species_A_Statistical_Manifold_Approach) #TODO

[Robust acoustic bird recognition for habitat monitoring with wireless sensor networks](https://link.springer.com/article/10.1007%2Fs10772-016-9354-4). Amira Boulmaiz. International Journal of Speech Technology, September 2016.
Tonal region detector (TRD) using sigmoid function is proposed.
Once the tonal regions in the noisy bird sound are detected, the features gammatone teager energy cepstral coefficients (GTECC).
TRD–GTECC.
Quantile-based cepstral dynamics normalization (QCN) for noise reduction. Extending ideas from computationally inexpensive normalizations of
spectral subtraction (SS), cepstral mean and variance (CMVN), and recently introduced cepstral gain normalization (CGN).
Compares MFCC, perceptual-MVDR (PMVDR) and power-normalized cepstral coefficients (PNCC).
Using different feature normalizations; SS, CMVN, CGN, and QCN.
GTECC had the best recognition rate, while being slightly less computationally intensive than MFCC.



[Vocal source separation using spectrograms and spikes, applied to speech and birdsong](https://www.research-collection.ethz.ch/handle/20.500.11850/175085). PhD thesis, ETH Zurich, 2017.
Audio source separation methods (ASS). Monaural source separation (MSS) special-case of ASS where only a single mixture is observed.
Spectral subtraction, Wiener filtering, and subspaces used in speech enhancement.
Ideal Binary Mask (IBM) used in auditory scene analysis (CASA).
Deep Neural Networks have been used to learn binary and soft masks, with state of art reslults.
This thesis presents novel linear and non-linear methods to address MSS in a supervised scenario

Three linear methods proposed in the thesis are:
1) Eigenmode analysis of covariance difference (EACD).
This method identifies spectro-temporal features associated with large
variance for one source and small variance for the other source.
2) Maximum likelihood demixing (MLD).
In this method, the mixture is modelled as the sum of two Gaussian signals
and maximum likelihood is used to identify the most likely sources.
3) Suppression-regression (SR).
Autoregressive models trained to reproduce one source but suppress the other.
4) A non-linear method called Multi-layered Random Forest (MLRF).
MLRF is an ensemble method that trains decision trees for each frequency band.
Given a mixture spectrogram, these trees classify individual T-F bin as belonging
to one of the speakers thus returning an estimate of the IBM.
An estimated IBM in a given layer is used to train a RF classifier in the next higher layer.
Outperforms a deep learning based method in terms of SNR of reconstructed audio.

[Birdsong Denoising Using Wavelets](https://www.ncbi.nlm.nih.gov/pmc/articles/PMC4728069/).
Using wavelets as alternative to bandpass. Not considering any source separation techniques.
Wavelet avoids the fundamental tradeoff between temporal and frequency resolution in Fourier spectrogram.
Nice background info on birdsong, including typical characteristics.

[Adaptive energy detection for bird sound detection in complex environments](https://www.sciencedirect.com/science/article/pii/S0925231214017068?via%3Dihub). Xiaoxia Zhang, Neurocomputing, 2015.
The noise spectrum of each band was estimated and the existent probability of the foreground bird sound for each band was computed to serve for the adaptive threshold of energy detection.
These foreground bird sound signals were detected and selected via adaptive energy detection from the bird sounds with background noises.
Features of Mel-scaled Wavelet packet decomposition Sub-band Cepstral Coefficient (MWSCC).
Moreover, the differences of recognition performance were implemented on 30 kinds of bird sounds at different Signal-to-Noise Ratios (SNRs)
under different noisy environments, before or after adaptive energy detection.
Classified with Support Vector Machine.


