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
  arena_allocator_t arena;
} pool_allocator_t;

/* Initializes a new pool allocator */
pool_allocator_t new_pool_allocator(size_t block_size);

/* Allocate a new block from the pool*/
void * poolalloc(pool_allocator_t * allocator);

/* Free a block */
void poolfree(pool_allocator_t * allocator, void * block);

/* Frees the entire pool allocator */
void delete_pool_allocator(pool_allocator_t * allocator);

#endif
