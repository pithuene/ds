#ifndef DS_POOL_H
#define DS_POOL_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

// The number of bits of a 32bit pool_cell_ref which identify a cell in a given pool.
// Every block contains 2^DS_POOL_CELL_BITS items.
#define DS_POOL_CELL_BITS 7
// The remaining number of bits in the pool_cell_ref.
// There can be up to 2^DS_POOL_BLOCK_BITS blocks.
#define DS_POOL_BLOCK_BITS 25

#ifndef DS_NO_SHORT_NAMES
  #define pool_allocator_t   ds_pool_allocator_t
  #define new_pool_allocator ds_new_pool_allocator
  #define poolalloc          ds_poolalloc
  #define poolfree           ds_poolfree
#endif /* DS_NO_SHORT_NAMES */

/* INTERNAL */

typedef struct {
  uint32_t block_idx : DS_POOL_BLOCK_BITS;
  uint32_t cell_idx  : DS_POOL_CELL_BITS;
} __ds_pool_cell_ref_t;

// The pool header essentially contains a cell ref,
// but is separate because it needs to have 8byte alignment
// to make sure no padding is inserted between the header
// and the first value.
typedef struct {
  uint64_t block_idx           : DS_POOL_BLOCK_BITS;
  uint64_t cell_idx            : DS_POOL_CELL_BITS;
  uint64_t number_of_blocks    : 32;
} __ds_pool_header_t;

void **__ds_new_pool_allocator(size_t val_len);

// Return the block index of the freelist head
uint32_t __ds_poolalloc_head_block_idx(void **allocator);
// Remove the freelist head from the freelist.
// Return the cell_idx of the removed head.
uint32_t __ds_poolalloc_head_cell_idx(void **allocator, size_t val_len);

void **__ds_pool_ensure_free_cell_internal(void **allocator, size_t val_len);

bool __ds_poolfree_internal(void **allocator, void *cell, size_t val_len);

// Make sure the freelist is not empty. Grow if necessary.
#define __ds_pool_ensure_free_cell(ALLOCATOR) \
  ((ALLOCATOR) = (void *) __ds_pool_ensure_free_cell_internal((void **) ALLOCATOR, sizeof(**ALLOCATOR)))

/* EXTERNAL */

#define ds_pool_allocator_t(TYPE) TYPE **

#define ds_new_pool_allocator(TYPE) \
  ((TYPE **) __ds_new_pool_allocator(sizeof(TYPE)))

#define ds_poolalloc(ALLOCATOR) ( \
  __ds_pool_ensure_free_cell(ALLOCATOR), \
  (&(ALLOCATOR) \
    [(__ds_poolalloc_head_block_idx((void **) ALLOCATOR))] \
    [(__ds_poolalloc_head_cell_idx((void **) ALLOCATOR, sizeof(**(ALLOCATOR))))]))

#define ds_poolfree(ALLOCATOR, CELL) \
  __ds_poolfree_internal((void **) ALLOCATOR, (void *) CELL, sizeof(**ALLOCATOR))

#endif
