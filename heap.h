#ifndef HEAP_H_
#define HEAP_H_

#include <stdbool.h>
#include <stdint.h>

#define HEAP_SIZE 		4096
#define CHUNK_OVERHEAD 	(sizeof(void*))

struct heapchunk {
	bool in_use;
	uint32_t size;
	/* Everything afterwards is unused if the chunk is in use; i.e, in_use is true. */
	struct heapchunk *next;
};

struct heap {
	struct heapchunk *head;
	uint32_t size;
	uint32_t space;
};

void *heap_alloc(uint32_t size);
void heap_free(void *ptr);

#endif
