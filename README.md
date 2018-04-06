# split_words
Code that inserts spaces in arbitrary string using dictionary of all possible words.

This exercise is frequently asked to be implemented during coding part of the SDE interview.
Implementation in this repo is much more sophisticated though. It covers:

* Preparation of english vocabulary of most frequent words with probabilities.
* Efficient implementation, that works for really long inputs.
* Code for evaluation of algorithm performance in terms of WER, %.
* Comparison of performance with different vocabulary sizes on the task of US Constitution splitting.

## Building

Simply run:

```
make
```
That will produce two binaries: `split_into_words` and `evaluate`.

## Usage

Input string should be piped to the binary. Repo contains 10k-words dictionary, to play with.

```
echo "codethatinsertsspacesinarbitrarystringusingdictionaryofallpossiblewords" | ./split_into_words word_list.10k.lst
```
Almost perfect split will be performed.

## Dictionary

Splitting algorithm relies on english vocabulary with probabilities.
Repo contains scripts to prepare such a vocabulary using [google ngram dataset](http://storage.googleapis.com/books/ngrams/books/datasetsv2.html).
Be careful, that takes a lot of disk space and some time.

```
cd vocab_creation
./download_google_ngrams.sh
./create_freq_dict.py . freq_dict.lst
./select_words.py freq_dict.lst word_list.1M.lst 1000000
# obtained word vocabulary is sorted by probability, so to limit the vocab
head -n 100000 word_list.1M.lst > word_list.100k.lst
```
## Performance

Performance of the split can be measured in terms of WER (Word Error Rate).
Binary for that is already build with `make`.

```
cat constitution.txt | tr -d '\n' | ./split_into_words word_list.10k.lst > split_result.txt
./evaluate constitution.txt split_result.txt
```
Performance of the algorithm with vocabularies of different size is presented in the table:

Vocabulary size  | WER, %
---------------- | -------------
10k              | 18.48
100k             | 6.89
1M               | 8.90[^1]

[^1]: The reason why bigger vocab is not better - it starts to contain concatenation of words, so result suffers from under-split.

## TODO
Some thoughts on how to improve the code and algo, but since it already works, I was lazy to do so.

* Current vocabulury is essentially 1-gram language model. Incorporating n-grams of higher orders would improve the performance.
* Currently the probability of the sequence is a sum of log-probs of separate words. That leads to P(rare\_long\_word) < P(short\_word1) + P(short\_word2)
* Sometimes for long text, all the hypothesis might be pruned out. That is because before adding next word, the subword does not decrease overall hypothesis probability. The issue can be also treated by increasing the trellis width, although it makes algo slower.

