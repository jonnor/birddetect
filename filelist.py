
import os
import sys

import pandas
import numpy

def main():
    path, out, dataset = sys.argv[1], sys.argv[2], sys.argv[3]
    files = os.listdir(path)    

    df = pandas.DataFrame({
        'itemid': [ f.replace('.wav', '') for f in files ],
        'datasetid': [dataset for f in files],
        'hasbird': [None for f in files],
    })
    df.to_csv(out, index=False)

if __name__ == '__main__':
    main()
