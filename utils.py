
import numpy
import dask.array

def sample_chunkwise(da, amount):
    chunk_size = da.chunks[0][0]
    chunks = len(da.chunks[0])

    chosen_chunks = numpy.random.random(size=chunks) > (1-amount)
    samples = []
    for i, chosen in enumerate(chosen_chunks):
        if chosen:
            indices = (i*chunk_size)+numpy.array(range(0, chunk_size))
            assert len(indices) == chunk_size 
            samples.append(indices)
    return numpy.concatenate(samples)
