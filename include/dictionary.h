
#include <string.h>
#include <assert.h>

#define CHARS_NUM 28

static int char2int(char symb) {
	int int_symb = (int)symb;
	if (symb == '\'') {
		return 26;
	}
	if (symb == '.') {
		return 27;
	}
	int_symb -= 97;
	if (!(int_symb < 26 && int_symb >= 0)) {
		fprintf(stderr, "**Error! Unsupported symbol [%c]\n", symb);
	}
	assert(int_symb < 26 && int_symb >= 0);
	return int_symb;
}

/*
 * Structure to keep dictionary.
 * Prefix-trie is implemented. Space-complexity
 * is smaller than hashset and 'contains' method
 * complexity is O(n), where n is the length of string
 * to search.
 */
typedef struct trie_node_s {
	struct trie_node_s *next_nodes;
	float prob;
} trie_node;

void trie_add(trie_node* trie, char *word, float prob) {
	int i;
	for (i = 0; i < strlen(word); i++) {
		char symb = word[i];
		int idx = char2int(symb);
		if (i == strlen(word) - 1) {
			trie[idx].prob = prob;
		} else {
			if (!trie[idx].next_nodes) {
				trie[idx].next_nodes = (trie_node*)calloc(CHARS_NUM, sizeof(trie_node));
			}
			trie = trie[idx].next_nodes;
		}
	}
}

float trie_get(trie_node* trie, char *word) {
	int i, idx;
	char symb;
	for (i = 0; i < strlen(word) - 1; i++) {
		symb = word[i];
		idx = char2int(symb);
		if (!trie[idx].next_nodes) {
			return 0;
		}
		trie = trie[idx].next_nodes;
	}
	symb = word[strlen(word) - 1];
	idx = char2int(symb);
	return trie[idx].prob;
}

void trie_free(trie_node *trie) {
	int i;
	for (i = 0; i < CHARS_NUM; i++) {
		if (trie[i].next_nodes) {
			trie_free(trie[i].next_nodes);
		}
	}
	free(trie);
}

trie_node* read_dict(char *path, float* floor_prob) {
	FILE *fp = fopen(path, "r");
	char buf[256];
	float min_prob = 0;
	trie_node* trie = (trie_node*)calloc(CHARS_NUM, sizeof(trie_node));
	while (fgets(buf, sizeof(buf), fp)) {
		int space_idx;
		char *word;
		float prob;
		for (space_idx = 0; space_idx < strlen(buf); space_idx++) {
			if (buf[space_idx] == ' ') {
				break;
			}
		}
		assert(space_idx > 0);
		buf[space_idx] = '\0';
		word = buf;
		prob = atof(&buf[space_idx + 1]);
		trie_add(trie, word, prob);
		if (prob < min_prob) {
			min_prob = prob;
		}
	}
	fclose(fp);
	*floor_prob = min_prob;
	return trie;
}
