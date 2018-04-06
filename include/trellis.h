
#include <stdio.h>

#include "array_list.h"

/*
 * Structure to keep node of Viterbi trellis.
 * Whole trellis - is just a list of such nodes,
 * which represent particular splitting hypothesis.
 */
typedef struct trellis_node_s {
	array_list *l;
	float prob;
} trellis_node;

void trellis_node_free(void *void_node) {
	trellis_node *node = (trellis_node *)void_node;
	array_list_free(node->l);
	free(node);
}

typedef struct trellis_s {
	void *ptr;
	struct trellis_s *next_ptr;
} trellis;

trellis* trellis_new(void *ptr) {
	trellis* l = (trellis *)calloc(1, sizeof(trellis));
	l->ptr = ptr;
	l->next_ptr = NULL;
	return l;
}

trellis *trellis_init() {
	trellis_node *init_node = (trellis_node *)calloc(1, sizeof(trellis_node));
	init_node->l = array_list_default_new();
	array_list_add_int(init_node->l, 0);
	return trellis_new(init_node);
}

trellis* trellis_add(trellis *l, void *ptr) {
	trellis* newl = trellis_new(ptr);
	newl->next_ptr = l;
	return newl;
}

trellis* trellis_prune(trellis *l, float prob) {
	trellis *iter;
	trellis *prev_l;

	/* drop first trellis node as much times as needed */
	while (((trellis_node *)l->ptr)->prob <= prob) {
		trellis *to_del = l;
		l = l->next_ptr;
		trellis_node_free(to_del->ptr);
		free(to_del);
	}

	prev_l = l;
	iter = l->next_ptr;

	while (iter) {
		if (((trellis_node *)iter->ptr)->prob <= prob) {
			/* Drop this node from the trellis list */
			prev_l->next_ptr = iter->next_ptr;
			trellis_node_free(iter->ptr);
			free(iter);
		} else {
			prev_l = iter;
		}
		iter = prev_l->next_ptr;
	}
	return l;
}

int trellis_size(trellis *l) {
	int size = 0;
	while (l) {
		size += 1;
		l = l->next_ptr;
	}
	return size;
}

void trellis_free(trellis *l) {
	while (l) {
		trellis* newl = l->next_ptr;
		trellis_node_free(l->ptr);
		free(l);
		l = newl;
	}
}

void trellis_print(trellis *l) {
	while (1) {
		int i;
		trellis_node *node = (trellis_node *)l->ptr;
		fprintf(stderr, "Hyphothesis: \n");
		fprintf(stderr, "Prob: %f\n", node->prob);
		fprintf(stderr, "Splits at: ");
		array_list_print_int(node->l);
		fprintf(stderr, "\n\n");
		l = l->next_ptr;
		if (!l) {
			break;
		}
	}
}

array_list* trellis_best_hyp(trellis *l, float *prob) {
	float maxprob = -999999999;
	trellis_node *best_node = NULL;
	while (l) {
		if (((trellis_node *)l->ptr)->prob > maxprob) {
			best_node = (trellis_node *)l->ptr;
			maxprob = best_node->prob;
		}
		l = l->next_ptr;
	}
	*prob = maxprob;
	return best_node->l;
}
