
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "edit_distance.h"


char** read_file(char *path, int *words_num) {
	char buf[128];
	char **words;

	/* compute number of words */
	FILE *fp = fopen(path, "r");
	int lines = 0;
	while (!feof(fp)) {
		if (!fgets(buf, 128, fp)) {
			continue;
		}
		lines += 1;
	}

	/* allocate words array */
	words = (char **)calloc(lines, sizeof(char*));

	/* rewind and read again */
	lines = 0;
	rewind(fp);
	while (!feof(fp)) {
		if (!fgets(buf, 128, fp)) {
			continue;
		}
		buf[strcspn(buf, "\r\n")] = 0;
		words[lines] = (char*)calloc(strlen(buf), sizeof(char));
		memcpy(words[lines], buf, strlen(buf));
		lines++;
	}

	fclose(fp);
	*words_num = lines;
	return words;
}

void free_words(char **words, int words_num) {
	int i;
	for (i = 0; i < words_num; i++) {
		if (words[i]) {
			free(words[i]);
		}
	}
	free(words);
}

int main(int argc, char **argv) {

	char **ref;
	char **hyp;
	int ref_num, hyp_num;
	int ins, del, sub;
	int i;

	if (argc != 3) {
		fprintf(stderr, "**Usage: ./evaluate <reference> <hypothesis>\n");
		return -1;
	}

	ref = read_file(argv[1], &ref_num);
	hyp = read_file(argv[2], &hyp_num);

	levenshtein_edit_distance(ref, hyp, ref_num, hyp_num, &ins, &del, &sub);

	fprintf(stderr, "WER: %.2f%%, insertions %d, deletions %d, substitutions %d\n",
			100.0 * (sub + del + ins) / ref_num, ins, del, sub);

	free_words(ref, ref_num);
	free_words(hyp, hyp_num);
}
