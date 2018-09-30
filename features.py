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

import itertools
def chunk_sequence(iterable, size):
    it = iter(iterable)
    item = list(itertools.islice(it, size))
    while item:
        yield item
        item = list(itertools.islice(it, size))

def extract_features(location, dataset,
                feature_extractor=None, feature_length=None, chunk_size=50):

    if feature_extractor is None:
        feature_extractor = extract_melspec_max
        feature_length = 64


    def wav_path(folder, item):
        return '/'.join((location, folder, 'wav', item+'.wav'))

    # Do processing in chunks, to avoid having too many tasks
    chunk_shape = (chunk_size, feature_length)
    print('chiunk', chunk_shape)
    def extract_chunk(urls):
        r = numpy.ndarray(shape=chunk_shape)
        for i, url in enumerate(urls):
            r[i,:] = feature_extractor(url)
        return r

    extract = dask.delayed(extract_chunk)
    def setup_extraction(urls):
        values = extract(urls)
        arr = dask.array.from_delayed(values, dtype=numpy.float, shape=chunk_shape)
        return arr

    urls = (wav_path(*t) for t in zip(dataset.folder, dataset.itemid))
    arrays = [ setup_extraction(c) for c in chunk_sequence(urls, chunk_size) ]
    features = dask.array.concatenate(arrays, axis=0)
    return features
    
features = extract_features(location, dataset)
features



import dask.distributed
c = dask.distributed.Client()
c

features[10:15,:]
