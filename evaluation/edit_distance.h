
#include <string.h>

typedef struct error_stats_s {
	int ins_num;
	int del_num;
	int sub_num;
	int total_cost;
} error_stats;

/* Mostly copy-paste from here:
 * https://github.com/kaldi-asr/kaldi/blob/master/src/util/edit-distance-inl.h */
void levenshtein_edit_distance(char **ref, char **hyp, int ref_num,
		int hyp_num, int *ins, int *del, int *sub) {
	error_stats* e = (error_stats *)calloc(ref_num + 1, sizeof(error_stats));
	error_stats* cur_e = (error_stats *)calloc(ref_num + 1, sizeof(error_stats));
	int i, hyp_index, ref_index;

	for (i = 0; i < ref_num + 1; i++) {
		e[i].ins_num = 0;
		e[i].sub_num = 0;
		e[i].del_num = i;
		e[i].total_cost = i;
	}

	// for other alignments
	for (hyp_index = 1; hyp_index <= hyp_num; hyp_index++) {
		cur_e[0] = e[0];
		cur_e[0].ins_num++;
		cur_e[0].total_cost++;
		for (ref_index = 1; ref_index <= ref_num; ref_index ++) {
			int ins_err = e[ref_index].total_cost + 1;
			int del_err = cur_e[ref_index-1].total_cost + 1;
			int sub_err = e[ref_index-1].total_cost;
			if (strcmp(hyp[hyp_index-1], ref[ref_index-1]) != 0)
				sub_err++;

			if (sub_err < ins_err && sub_err < del_err) {
				cur_e[ref_index] = e[ref_index-1];
				if (strcmp(hyp[hyp_index-1], ref[ref_index-1]) != 0)
					cur_e[ref_index].sub_num++;	// substitution error should be increased
				cur_e[ref_index].total_cost = sub_err;
			} else if (del_err < ins_err) {
				cur_e[ref_index] = cur_e[ref_index-1];
				cur_e[ref_index].total_cost = del_err;
				cur_e[ref_index].del_num++;		// deletion number is increased.
			} else {
				cur_e[ref_index] = e[ref_index];
				cur_e[ref_index].total_cost = ins_err;
				cur_e[ref_index].ins_num++;		// insertion number is increased.
			}
		}
		memcpy(e, cur_e, sizeof(error_stats) * (ref_num + 1));
	}

	*ins = e[ref_num].ins_num;
	*del = e[ref_num].del_num;
	*sub = e[ref_num].sub_num;

	free(e);
	free(cur_e);
}
