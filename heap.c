/*
 * TODO: Add chunk coalescing.
 */

#include <stdio.h>
#include <windows.h>

#include "heap.h"

static struct heap *heap;

__attribute__((constructor))
static void heap_init() {
	struct heapchunk *head = NULL;

	heap = VirtualAlloc(NULL, HEAP_SIZE, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
	if (heap == NULL) {
		fprintf(stderr, "ERROR: VirtualAlloc\n");
		exit(1);
	}

	heap->size = HEAP_SIZE;
	heap->space = HEAP_SIZE - sizeof(struct heap) - sizeof(struct heapchunk) + CHUNK_OVERHEAD;
	heap->head = (struct heapchunk*)(heap + 1);

	head = heap->head;
	head->in_use = false;
	head->size = heap->space;
	head->next = NULL;

	printf("Heap Initialized at %p.\n", (void*)heap);
}

__attribute__((destructor))
static void heap_fini() {
	VirtualFree(heap, heap->size, MEM_DECOMMIT | MEM_RELEASE);

	printf("Heap Finished.\n");
	heap = NULL;
}

void *heap_alloc(uint32_t size) {
	if (size + sizeof(struct heapchunk) - CHUNK_OVERHEAD > heap->space) {
		fprintf(stderr, "ERROR: Not enough space on the heap.\n");
		return NULL;
	}

	void *retptr = NULL;
	struct heapchunk *old_chunk = NULL;
	struct heapchunk *new_chunk = NULL;
	struct heapchunk *old_next = NULL;
	uint32_t old_size = 0;

	for (old_chunk = heap->head; old_chunk != NULL; old_chunk = old_chunk->next) {
		if (old_chunk->size > size + sizeof(struct heapchunk) - CHUNK_OVERHEAD)
			break;
	}

	if (old_chunk == NULL) {
		fprintf(stderr, "ERROR: Couldn't find a chunk big enough.\n");
		return NULL;
	}

	old_size = old_chunk->size;
	old_next = old_chunk->next;

	new_chunk = (struct heapchunk*)((char*)old_chunk + size - CHUNK_OVERHEAD + sizeof(struct heapchunk));
	new_chunk->size = old_size - size - sizeof(struct heapchunk) + CHUNK_OVERHEAD;
	new_chunk->in_use = false;
	new_chunk->next = old_next;

	old_chunk->in_use = true;
	old_chunk->size = size;

	heap->space -= size + sizeof(struct heapchunk) - CHUNK_OVERHEAD;
	heap->head = new_chunk;

	retptr = (void*)((char*)old_chunk + sizeof(struct heapchunk) - CHUNK_OVERHEAD);

	for (uint32_t i = 0; i < size; i++)
		((char*)retptr)[i] = '\0';

	printf("Allocated %u bytes of memory at %p.\n", size, retptr);

	return retptr;
}

void heap_free(void *ptr) {
	struct heapchunk *chunk = (struct heapchunk*)((char*)ptr - (sizeof(struct heapchunk) - CHUNK_OVERHEAD));

	chunk->in_use = false;
	chunk->next = heap->head;
	heap->head = chunk;

	printf("Freed %d bytes at %p\n", chunk->size, ptr);
}
