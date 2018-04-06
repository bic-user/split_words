/*
 * Strucuture to keep store array of arbitrary length.
 * This is essentially an ArrayList, where actual length is doubled
 * if needed. Could store either int or float using union.
 */
typedef struct array_list_s {
	union arr_element_u {
		int i;
		float f;
	} *arr;
	int size;
	int actual_size;
} array_list;

typedef union arr_element_u arr_element;

#define DEFAULT_CAPACITY 10

array_list* array_list_new(int capacity) {
	array_list *l = (array_list *)calloc(1, sizeof(array_list));
	l->arr = (arr_element *)malloc(capacity * sizeof(arr_element));
	l->actual_size = capacity;
	return l;
}

array_list* array_list_default_new() {
	return array_list_new(DEFAULT_CAPACITY);
}

void array_list_add(array_list *l, arr_element i) {
	if (l->size == l->actual_size) {
		l->actual_size;
		arr_element *new_arr = (arr_element *)calloc(l->actual_size * 2,
			sizeof(arr_element));
		memcpy(new_arr, l->arr, sizeof(arr_element) * l->actual_size);
		free(l->arr);
		l->arr = new_arr;
		l->actual_size *= 2;
	}
	l->arr[l->size] = i;
	l->size += 1;
}

#define array_list_add_int(l, i) array_list_add(l, (arr_element)i)

arr_element array_list_last(array_list *l) {
	return l->arr[l->size - 1];
}

#define array_list_last_int(l) (array_list_last(l)).i

array_list* array_list_copy(array_list *l) {
	array_list *newl = (array_list *)calloc(1, sizeof(array_list));
	newl->arr = (arr_element *)malloc(l->actual_size * sizeof(arr_element));
	memcpy(newl->arr, l->arr, sizeof(arr_element) * l->actual_size);
	newl->actual_size = l->actual_size;
	newl->size = l->size;
	return newl;
}

void array_list_free(array_list *l) {
	if (l->arr) {
		free(l->arr);
	}
	free(l);
}

void array_list_print_int(array_list *l) {
	int i;
	for (i = 0; i < l->size; i++) {
		fprintf(stderr, "%d ", l->arr[i].i);
	}
}
