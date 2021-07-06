#ifndef DS_ARENAALLOC_H
#define DS_ARENAALLOC_H

#include <sys/mman.h>
#include "vec.h"

/**
 * Each container is represented by a pointer to its beginning.
 * The number of bytes allocated from this container is stored as a size_t in
 * front of the pointer.
 */

typedef struct {
  size_t container_size;
  void ** containers;
} ArenaAllocator;

/* Initializes a new arena allocator */
ArenaAllocator newArenaAllocator(size_t container_size);

/* Allocate size bytes from an arena allocator */
void * arenaalloc(ArenaAllocator * allocator, size_t size);

/* Frees the entire arena */
void deleteArenaAllocator(ArenaAllocator * allocator);

#endif
