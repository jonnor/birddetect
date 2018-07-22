
import sys
import os

import simpleaudio
import numpy
import pandas


datadir='data/full'
datasets = {
    'birdvox': ('birdvox/birdvox.labels.csv','birdvox/wav'),
    'ff1010bird': ('ff1010bird/ff1010bird.labels.csv','ff1010bird/wav'),
    'warbrl_train': ('warblrb10k_public/warblrb10k.labels.csv','warblrb10k_public/wav'),    
    'chern': ('chern/chern.files.csv','chern/chern_wav'),
    'polandnfc': ('polandnfc/polandnfc.files.csv','polandnfc/PolandNFC_test_wav'),
    'warbrl_test': ('wabrlrb10k_test/files.csv','wabrlrb10k_test/wabrlrb10k_test_wav/'),
}
trainsets = ['birdvox','ff1010bird','warbrl_train']
testsets = set(datasets.keys()) - set(trainsets)

def main():
    inp, out, offset = sys.argv[1], sys.argv[2], int(sys.argv[3])

    sampled = pandas.read_csv(inp, index_col=0)
    tolabel = sampled[sampled.hasbird.isnull()]
    files = list(tolabel.index)[offset:]

    filenames = []
    has_birds = []

    print(len(files))
    assert len(files) > 0

    try:
        for i, item in enumerate(files):
            dataset = tolabel.dataset[item]
            datasetdir = datasets[dataset][1]
            path = os.path.join(datadir, datasetdir, item+'.wav')

            print('{} playing: {}', i, path)
            wav = simpleaudio.WaveObject.from_wave_file(path)
            play = wav.play()
            play.wait_done()

            print("Did file contain bird? y/n")
            inp = input()
            has_bird = None
            if inp not in ('y', 'n'):
                print("ERROR: got", inp)
            else:
                has_bird = inp == 'y'

            print('GT: {},{}', path, has_bird)

            filenames.append(path)
            has_birds.append(has_bird)

    except KeyboardInterrupt:
        df = pandas.DataFrame({
            'itemid': [ os.path.basename(f).replace('.wav', '') for f in filenames ],
            'hasbird': has_birds,
        })
        df.to_csv(out, index=False)

if __name__ == '__main__':
    main()
