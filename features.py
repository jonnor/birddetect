import io
import urllib.request

import numpy
import pandas
import dask.array

import scipy.io.wavfile
import librosa


def read_audio(path):
    f = urllib.request.urlopen(path)
    data = io.BytesIO(f.read())
    samplerate, samples  = scipy.io.wavfile.read(data)
    assert samplerate == 44100, samplerate
    assert samples.shape[0] >= samplerate*(10-0.1), samples.shape  # should be roughly 10 secs
    
    return samplerate, samples

def meansub(s):
    return s - (numpy.mean(s, axis=0) + 1e-8)

def minmaxscale(s):
    mins = numpy.min(s, axis=0) + 1e-8
    maxs = numpy.max(s, axis=0)
    return ( s - mins) / ( maxs - mins )

def melspec_maxp(data, sr):
    params = dict(n_mels=64, fmin=500, n_fft=2048, fmax=15000, htk=True)
    mel = librosa.feature.melspectrogram(y=data, sr=sr, **params)

    # TODO: make normalization configurable
    mel = meansub(mel)
    mel = minmaxscale(mel)

    features = numpy.concatenate([
        numpy.max(mel, axis=1),
    ])
    return features

def extract_melspec_max(path):
    samplerate, samples = read_audio(path)
    return melspec_maxp(samples.astype(float), samplerate)

f = extract_melspec_max('https://storage.googleapis.com/dcase2018-bad/birdvox/wav/00053d90-e4b9-4045-a2f1-f39efc90cfa9.wav')
f.shape

# returns Pandas.DataFrame
def load_dataset(location):
    folders = [
        'polandnfc',
        'birdvox',
        'chern',
        'ff1010bird',
        'warblr10k_public',
        'warblr10k_test',
    ]

    def load_folder(folder):
        u = '/'.join((location, folder, 'files.csv'))
        #f = urllib.request.urlopen(u)
        df = pandas.read_csv(u, dtype={'itemid': str})
        assert df.itemid.dtype == 'object', df.itemid.dtype
        df['folder'] = folder
        return df
    
    df = pandas.concat([load_folder(f) for f in folders])
    return df
        

location = 'https://storage.googleapis.com/dcase2018-bad'
dataset = load_dataset(location)

print(dataset.shape)
dataset.head(3)

def extract_features(location, dataset,
                feature_extractor=None, feature_shape=None):

    if feature_extractor is None:
        feature_extractor = extract_melspec_max
        feature_shape = (64,)

    def wav_path(folder, item):
        return '/'.join((location, folder, 'wav', item+'.wav'))
    
    extract = dask.delayed(feature_extractor)
    def setup_extraction(folder, item):
        url = wav_path(folder, item)
        value = extract(url)
        arr = dask.array.from_delayed(value, dtype=numpy.float, shape=feature_shape)
        return arr

    arrays = [ setup_extraction(*t) for t in zip(dataset.folder, dataset.itemid) ]
    features = dask.array.stack(arrays, axis=0)
    return features
    
features = extract_features(location, dataset)
features

