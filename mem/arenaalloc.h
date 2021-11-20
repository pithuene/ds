#ifndef DS_ARENAALLOC_H
#define DS_ARENAALLOC_H

#include <sys/mman.h>
#include "../ds/vec.h"

/**
 * Each container is represented by a pointer to its beginning.
 * The number of bytes allocated from this container is stored as a size_t in
 * front of the pointer.
 */

typedef struct {
  size_t container_size;
  vec_t(void *) containers;
} arena_allocator_t;

/* Initializes a new arena allocator */
arena_allocator_t new_arena_allocator(size_t container_size);

/* Allocate size bytes from an arena allocator */
void * arenaalloc(arena_allocator_t * allocator, size_t size);

/* Frees the entire arena */
void free_arena_allocator(arena_allocator_t * allocator);

#endif
