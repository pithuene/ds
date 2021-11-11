#ifndef DS_POOLALLOC_H
#define DS_POOLALLOC_H

#include <unistd.h>
#include "arenaalloc.h"

/**
 * Provides a pool of fixed size blocks.
 * Free blocks are connected using a free-list.
 */

typedef struct {
  size_t block_size;
  void * free_list_head;
  ArenaAllocator arena;
} PoolAllocator;

/* Initializes a new pool allocator */
PoolAllocator newPoolAllocator(size_t block_size);

/* Allocate a new block from the pool*/
void * poolalloc(PoolAllocator * allocator);

/* Free a block */
void poolfree(PoolAllocator * allocator, void * block);

/* Frees the entire pool allocator */
void deletePoolAllocator(PoolAllocator * allocator);

#endif
