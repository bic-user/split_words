
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "dictionary.h"
#include "trellis.h"


#define MAX_INSTR_LEN 50000
#define MAX_WORD_LEN 20
#define MAX_TRELLIS_WIDTH 6000


static int float_comp(const void *a, const void *b) {
	float aa = *((float *)a);
	float bb = *((float *)b);
	if (aa > bb) return 1;
	if (aa < bb) return -1;
	return 0;
}


static float unseen_prob(float floor_prob, int word_len) {
	return floor_prob - 100.0 * exp((word_len / (double)MAX_WORD_LEN) - 1);
}


int main(int argc, char **argv) {

	int i;

	trie_node *vocab;
	float floor_prob;
	char word[MAX_WORD_LEN];

	char instr[MAX_INSTR_LEN];

	trellis *hyps;
	float probs[MAX_TRELLIS_WIDTH * 4];
	int probs_num;

	array_list *split;
	float split_prob;

	if (argc == 1 || strcmp(argv[1], "-h") == 0 ||
				strcmp(argv[1], "--help") == 0) {
		fprintf(stderr, "Usage: echo \"helloworld\" |"
				" ./split_into_words <word-list>\n");
		return -1;
	}

	/* read vocab with word probs */
	vocab = read_dict(argv[1], &floor_prob);

	/* read string for splitting */
	if (!fgets(instr, MAX_INSTR_LEN, stdin)) {
		fprintf(stderr, "**Error! Failed to read str for split from stdin\n");
		return -1;
	}

	/* init the trellis with hypothesis */
	hyps = trellis_init();

	/* iterate over input string, filling trellis */
	for (i = 1; i < strlen(instr); i++) {

		/* iterate over trellis, allowing either insert white-space or not
		 * after current character for all availalbe hypothesis */
		trellis *new_hyps = NULL;
		trellis *last_new_hyps;
		trellis *hyps_iter = hyps;
		trellis *hyps_iter_prev = NULL;
		probs_num = 0;

		if (!hyps) {
			fprintf(stderr, "**Error! All pruned out. Try bigger MAX_TRELLIS_WIDTH\n");
			return -1;
		}

		while(hyps_iter) {
			trellis *new_hyps_inserted;
			trellis_node *node = (trellis_node *)hyps_iter->ptr;
			int from_i = array_list_last_int(node->l);
			int word_len = i - from_i;
			float prob;
			trellis_node *new_node;

			if (word_len >= MAX_WORD_LEN) {
				/* Too long word in this hyphotesis, it should be removed */
				trellis *to_del;
				if (!hyps_iter_prev) {
					to_del = hyps;
					hyps = hyps->next_ptr;
					hyps_iter = hyps_iter->next_ptr;
				} else {
					to_del = hyps_iter;
					hyps_iter = hyps_iter->next_ptr;
					hyps_iter_prev->next_ptr = hyps_iter;
				}
				trellis_node_free(to_del->ptr);
				free(to_del);
				continue;
			}

			/* get the probability of hypothesized word */
			strncpy(word, instr + from_i, word_len);
			word[word_len] = '\0';
			prob = trie_get(vocab, word);
			if (prob >= 0) {
				/* Unseen word, needs to compose it's prob, basing on it's len */
				prob = unseen_prob(floor_prob, word_len);
			}
			probs[probs_num++] = prob;
			probs[probs_num++] = node->prob;

			/* add hypothesis into newly composed trellis */
			new_node = (trellis_node *)calloc(1, sizeof(trellis_node));
			new_node->l = array_list_copy(node->l);
			array_list_add_int(new_node->l, i);
			new_node->prob = node->prob + prob;
			new_hyps_inserted = trellis_add(new_hyps, (void *)new_node);
			if (!new_hyps) {
				/* Store for concatenation */
				last_new_hyps = new_hyps_inserted;
			}
			new_hyps = new_hyps_inserted;

			/* update hypothesis iterator */
			hyps_iter_prev = hyps_iter;
			hyps_iter = hyps_iter->next_ptr;
		}

		/* add concatenate newly create trellis to original one */
		if (i == strlen(instr) - 1) {
			trellis_free(hyps);
			hyps = new_hyps;
		} else {
			last_new_hyps->next_ptr = hyps;
			hyps = new_hyps;
		}

		/* perform pruning of the trellis if needed */
		if (probs_num >= MAX_TRELLIS_WIDTH && i < (strlen(instr) - 1)) {
			float median_prob;
			float prune_aggresiveness;
			qsort(probs, probs_num, sizeof(float), float_comp);
			if (probs_num <= 2 * MAX_TRELLIS_WIDTH) {
				prune_aggresiveness = 0.3;
			} else {
				prune_aggresiveness = 0.5;
			}
			median_prob = probs[(int)(probs_num * prune_aggresiveness)];
			hyps = trellis_prune(hyps, median_prob);
		}

		/* print progress */
		fprintf(stderr, "\r#Progress: %.2f", 100.0 * i / strlen(instr));
	}

	/* Print out most probable split */
	split = trellis_best_hyp(hyps, &split_prob);
	fprintf(stderr, "\n#Split with prob %f\n", split_prob);
	for (i = 1; i < split->size; i++) {
		int word_start = split->arr[i - 1].i;
		int word_end = split->arr[i].i;
		int word_len = word_end - word_start;
		strncpy(word, instr + word_start, word_len);
		word[word_len] = '\0';
		printf("%s\n", word);
	}

	/* free resources */
	trellis_free(hyps);
	trie_free(vocab);

	return 0;
}
