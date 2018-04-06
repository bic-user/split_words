#!/usr/bin/env bash

for i in a b c d e f g h i j k l m n o p q r s t u v w x y z; do
	wget http://storage.googleapis.com/books/ngrams/books/googlebooks-eng-all-1gram-20120701-$i.gz;
done
