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

def test(path):
    f = urllib.request.urlopen(path)
    data = io.BytesIO(f.read())
    samplerate, samples  = scipy.io.wavfile.read(data)
    assert samplerate == 44100, samplerate
    assert samples.shape[0] >= samplerate*(10-0.1), samples.shape  # should be roughly 10 secs
    
    f = melspec_maxp(samples.astype(float), samplerate)
    return f

f = test('https://storage.googleapis.com/dcase2018-bad/birdvox/wav/00053d90-e4b9-4045-a2f1-f39efc90cfa9.wav')
f.shape


