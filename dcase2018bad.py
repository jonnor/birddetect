
import itertools
import urllib.request


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

# Return URL for a single wav file
def wav_url(folder, item, location=None):
    if location is None:
        location = default_location

    return '/'.join((location, folder, 'wav', item+'.wav'))

# Return URLs for each wav file in dataset 
def wav_urls(dataset, location=None):
    urls = (wav_url(*t, location=location) for t in zip(dataset.folder, dataset.itemid))
    return urls

