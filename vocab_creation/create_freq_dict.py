#!/usr/bin/env python3

import os
import sys
import gzip
import re
from glob import glob
from multiprocessing import Pool


def create_vocab(path):
    line_num = 0
    vocab = {}
    with gzip.open(path, 'r') as infp:
        for line in infp:
            line_num += 1
            line = line.strip()
            parts = line.split()
            word = parts[0].decode().split('_')[0].lower()
            count = int(parts[2])

            if all((c.isalpha() or c == '.' or c == '\'') for c in word):
                if word in vocab:
                    vocab[word] += count
                else:
                    vocab[word] = count

            if line_num % 100000 == 0:
                print(line_num)

    return vocab


if __name__ == '__main__':

    if len(sys.argv) != 3:
        raise RuntimeError('Usage: <dir-with-ngrams> <out-dict>')

    pool = Pool(processes=8)
    results = pool.map(create_vocab, glob('%s/*.gz' % sys.argv[1]))
    pool.close()
    pool.join()

    with open(sys.argv[2], 'w') as outfp:
        for vocab in results:
            for word, count in vocab.items():
                outfp.write('%s %d\n' % (word, count))

