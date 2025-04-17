#include <stdio.h>

#include "heap.h"

int main(void) {
    int *int_array = heap_alloc(100 * sizeof(int));
    char *str = heap_alloc(40 * sizeof(char));

    heap_free(int_array);
    heap_free(str);

	return 0;
}