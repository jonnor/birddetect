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
    try:
        samplerate, samples = read_audio(path)
    except AssertionError:
        return numpy.full((64,), numpy.nan)
    return melspec_maxp(samples.astype(float), samplerate)



import itertools
def chunk_sequence(iterable, size):
    it = iter(iterable)
    item = list(itertools.islice(it, size))
    while item:
        yield item
        item = list(itertools.islice(it, size))


# Return a Dask.array, for distributed lazy computation of features
def extract(urls, location=None,
            feature_extractor=None, feature_length=None, chunk_size=50):

    if feature_extractor is None:
        feature_extractor = extract_melspec_max
        feature_length = 64

    # Do processing in chunks, to avoid having too many tasks
    chunk_shape = (chunk_size, feature_length)
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

    arrays = [ setup_extraction(c) for c in chunk_sequence(urls, chunk_size) ]
    features = dask.array.concatenate(arrays, axis=0)
    return features


