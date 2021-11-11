#include "poolalloc.h"

PoolAllocator newPoolAllocator(size_t block_size) {
  // Make sure a block can hold a free-list pointer
  if (block_size < sizeof(size_t)) {
    block_size = sizeof(size_t);
  }


  int page_size = getpagesize();
  // Make sure a block fits inside a pool
  if (block_size > page_size) {
    int pages_per_block = (block_size / page_size) + 1;
    page_size *= pages_per_block;
  }

  PoolAllocator pool_allocator;
  pool_allocator.block_size = block_size;
  ArenaAllocator arena = newArenaAllocator(page_size);
  pool_allocator.arena = arena;
  pool_allocator.free_list_head = NULL;

  return pool_allocator;
}

void * poolalloc(PoolAllocator * pool_allocator) {
  if (pool_allocator->free_list_head != NULL) {
    void * block = pool_allocator->free_list_head;
    // Remove from free-list
    pool_allocator->free_list_head = *((void **) pool_allocator->free_list_head);
    return block;
  } else {
    // Free-list empty
    void * block = arenaalloc(&pool_allocator->arena, pool_allocator->block_size);
    return block;
  }
}


void poolfree(PoolAllocator * allocator, void * block) {
  *((void **) block) = allocator->free_list_head;
  allocator->free_list_head = block;
}

void deletePoolAllocator(PoolAllocator * allocator) {
  deleteArenaAllocator(&allocator->arena);
}
