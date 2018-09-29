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
# create Dask array over all the sound files
def get_dataset(location='https://storage.googleapis.com/dcase2018-bad',
                feature_extractor=None, feature_shape=None):

    if feature_extractor is None:
        feature_extractor = extract_melspec_max
        feature_shape = (64,)

    def get_filelist(dataset_name):
        u = '/'.join((location, dataset_name, 'files.csv'))
        #f = urllib.request.urlopen(u)
        df = pandas.read_csv(u, dtype={'itemid': str})
        assert df.itemid.dtype == 'object', df.itemid.dtype
        return df.itemid.values, df.hasbird.values
    def wav_path(dataset, item):
        return '/'.join((location, dataset, 'wav', item+'.wav'))
    
    def load_folder(dataset_name):
        items, labels = get_filelist(dataset_name) # NOTE: eagerly, since we need the size
        urls = [wav_path(dataset_name, n) for n in items]

        func = dask.delayed(feature_extractor)
        lazy_values = [ func(url) for url in urls ]

        # Construct a small Dask array for each of the loaded files
        arrays = [dask.array.from_delayed(v, dtype=numpy.float, shape=feature_shape) for v in lazy_values]
        # Stack all small Dask arrays into one
        stack = dask.array.stack(arrays, axis=0)
        return stack, labels
    
    folders = [
        'polandnfc',
        'birdvox',
        'chern',
        'ff1010bird',
        'warblr10k_public',
        'warblr10k_test',
    ]

    features = []
    labels = []
    for name in folders:
        f, l = load_folder(name)
        features.append(f)
        labels.append(l)

    features = dask.array.concatenate(features)
    labels = dask.array.concatenate(labels)
    return features, labels

d, l = get_dataset()
d.shape, l.shape

