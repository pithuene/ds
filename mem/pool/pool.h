#ifndef DS_POOL_H
#define DS_POOL_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

// The number of bits of a 32bit pool_cell_ref which identify a cell in a given
// pool. Every block contains 2^DS_POOL_CELL_BITS items.
#define DS_POOL_CELL_BITS 7
// The remaining number of bits in the pool_cell_ref.
// There can be up to 2^DS_POOL_BLOCK_BITS blocks.
#define DS_POOL_BLOCK_BITS 25

#ifndef DS_NO_SHORT_NAMES
#define pool_allocator_t             ds_pool_allocator_t
#define pool_allocator_create        ds_pool_allocator_create
#define pool_allocator_delete        ds_pool_allocator_delete
#define poolalloc                    ds_poolalloc
#define poolalloc_cellref            ds_poolalloc_cellref
#define poolfree                     ds_poolfree
#define poolfree_cellref             ds_poolfree_cellref
#define pool_allocator_freelist_head ds_pool_allocator_freelist_head
#endif /* DS_NO_SHORT_NAMES */

/* INTERNAL */

typedef struct {
  uint32_t block_idx : DS_POOL_BLOCK_BITS;
  uint32_t cell_idx : DS_POOL_CELL_BITS;
} __ds_pool_cell_ref_t;

extern __ds_pool_cell_ref_t __ds_pool_NULL_CELL_REF;

// The pool header essentially contains a cell ref,
// but is separate because it needs to have 8byte alignment
// to make sure no padding is inserted between the header
// and the first value.
typedef struct {
  uint64_t block_idx : DS_POOL_BLOCK_BITS;
  uint64_t cell_idx : DS_POOL_CELL_BITS;
  uint64_t number_of_blocks : 32;
} __ds_pool_header_t;

void **__ds_pool_allocator_create(size_t val_len);

// Return the block index of the freelist head
uint32_t __ds_poolalloc_head_block_idx(void **allocator);
// Remove the freelist head from the freelist.
// Return the cell_idx of the removed head.
uint32_t __ds_poolalloc_head_cell_idx(void **allocator, size_t val_len);

void **__ds_pool_ensure_free_cell_internal(void **allocator, size_t val_len);

bool __ds_poolfree_internal(void **allocator, void *cell, size_t val_len);

void __ds_poolfree_cellref_internal(
  void **allocator, __ds_pool_cell_ref_t cellref, void *cell
);

// Make sure the freelist is not empty. Grow if necessary.
#define __ds_pool_ensure_free_cell(ALLOCATOR)                  \
  ((ALLOCATOR) = (void *) __ds_pool_ensure_free_cell_internal( \
     (void **) ALLOCATOR, sizeof(**ALLOCATOR)                  \
   ))

__ds_pool_cell_ref_t __ds_poolalloc_cellref_internal(
  void **allocator, size_t val_len
);

/* EXTERNAL */

#define ds_pool_allocator_t(TYPE) TYPE **

#define ds_pool_allocator_create(TYPE) \
  ((TYPE **) __ds_pool_allocator_create(sizeof(TYPE)))

#define ds_poolalloc(ALLOCATOR)                                               \
  (__ds_pool_ensure_free_cell(ALLOCATOR),                                     \
   (&(ALLOCATOR)[(__ds_poolalloc_head_block_idx((void **) ALLOCATOR))][(      \
     __ds_poolalloc_head_cell_idx((void **) ALLOCATOR, sizeof(**(ALLOCATOR))) \
   )]))

// Allocate but return a cellref instead of a pointer
#define ds_poolalloc_cellref(ALLOCATOR)   \
  (__ds_pool_ensure_free_cell(ALLOCATOR), \
   __ds_poolalloc_cellref_internal((void **) ALLOCATOR, sizeof(**ALLOCATOR)))

#define ds_poolfree(ALLOCATOR, CELL)                        \
  __ds_poolfree_internal(                                   \
    (void **) ALLOCATOR, (void *) CELL, sizeof(**ALLOCATOR) \
  )

#define ds_poolfree_cellref(ALLOCATOR, CELLREF)       \
  __ds_poolfree_cellref_internal(                     \
    (void **) ALLOCATOR,                              \
    CELLREF,                                          \
    &(ALLOCATOR)[CELLREF.block_idx][CELLREF.cell_idx] \
  )

void ds_pool_allocator_delete(void *allocator);

__ds_pool_cell_ref_t ds_pool_allocator_freelist_head(void *allocator);

#endif
