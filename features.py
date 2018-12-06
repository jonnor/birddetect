import io
import urllib.request

import numpy
import pandas
import dask.array


import itertools
def chunk_sequence(iterable, size):
    it = iter(iterable)
    item = list(itertools.islice(it, size))
    while item:
        yield item
        item = list(itertools.islice(it, size))


# Return a Dask.array, for distributed lazy computation of features
def extract(wavfiles, feature_extractor, feature_shape, dtype=numpy.float, chunk_size=10):

    # Do processing in chunks, to avoid having too many tasks
    chunk_shape = (chunk_size, feature_shape[0], feature_shape[1])
    def extract_chunk(urls):
        r = numpy.zeros(shape=chunk_shape, dtype=dtype)
        for i, url in enumerate(urls):
            r[i,:] = feature_extractor(url)
        return r

    extract = dask.delayed(extract_chunk)
    def setup_extraction(urls):
        values = extract(urls)
        arr = dask.array.from_delayed(values, dtype=dtype, shape=chunk_shape)
        return arr

    arrays = [ setup_extraction(c) for c in chunk_sequence(wavfiles, chunk_size) ]
    features = dask.array.concatenate(arrays, axis=0)

    return features

