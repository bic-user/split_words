CC=gcc

all: clean split_into_words evaluate

split_into_words:
	$(CC) -O3 -o split_into_words split_into_words.c -Iinclude -lm

evaluate:
	$(CC) -O3 -o evaluate evaluation/evaluate.c -Ievaluation

clean:
	$(RM) split_into_words evaluate
