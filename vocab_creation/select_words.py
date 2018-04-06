#!/usr/bin/env python3

import os
import re
import sys
import math


if __name__ == '__main__':

    if len(sys.argv) != 4:
        raise RuntimeError('Usage: <freq-dict> <word-list> <top-n-words>')

    word_re = re.compile('^[a-z\.\']+$')

    vocab = []
    total_count = 0
    with open(sys.argv[1], 'r') as infp:
        for line in infp:
            line = line.rstrip()
            word, count_str = line.split()
            if not word_re.match(word):
                continue
            count = int(count_str)
            vocab.append((word, count))
            total_count += count

    vocab.sort(key=lambda x: x[1], reverse=True)

    with open(sys.argv[2], 'w') as outfp:
        for word, count in vocab[:int(sys.argv[3])]:
            count = math.log10(count / float(total_count))
            outfp.write('%s %f\n' % (word, count))

