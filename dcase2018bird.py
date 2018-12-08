
import itertools
import urllib.request
import os.path

import pandas
import numpy
import dask
import dask.array


default_location = 'https://storage.googleapis.com/dcase2018-bad'


def chunk_sequence(iterable, size):
    it = iter(iterable)
    item = list(itertools.islice(it, size))
    while item:
        yield item
        item = list(itertools.islice(it, size))

# returns Pandas.DataFrame for whole dataset
def load_dataset(location=None):
    if location is None:
        location = default_location
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
        df = pandas.read_csv(u, dtype={'itemid': str})
        assert df.itemid.dtype == 'object', df.itemid.dtype
        df['folder'] = folder
        return df
    
    df = pandas.concat([load_folder(f) for f in folders])
    return df

# Return wav URL for a single sample
def wav_url(folder, item, location=None):
    if location is None:
        location = default_location

    return '/'.join((location, folder, 'wav', item+'.wav'))

# Return wav URLs for each sample file in dataset 
def wav_urls(dataset, location=None):
    urls = (wav_url(*t, location=location) for t in zip(dataset.folder, dataset.itemid))
    return urls

# Return URL for a single feature file
def feature_url(folder, itemid, feature, location=None, ext=None):
    if location is None:
        location = default_location+'/features'
    if ext is None:
        ext = '.png'
    return os.path.join(location, feature, folder, itemid+ext)

# Return URLs for each sample in dataset
def feature_urls(dataset, feature,
                 location=None):
    
    items = zip(dataset.folder, dataset.itemid)
    urls = [ feature_url(*t, feature, location) for t in items ]
    return urls

