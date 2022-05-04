#include "pool.h"
#include <stdlib.h>
#include <assert.h>

/* NOT EXPOSED */

const __ds_pool_cell_ref_t NULL_CELL_REF = {
  .block_idx = 0xFFFFFFFF & ((1 << DS_POOL_BLOCK_BITS) - 1),
  .cell_idx  = 0xFFFFFFFF & ((1 << DS_POOL_CELL_BITS) - 1),
};

static inline size_t block_arr_memory_len(size_t number_of_blocks) {
  return sizeof(__ds_pool_header_t) + sizeof(void *) * number_of_blocks;
}

// Returns a pointer to the first block pointer for a given pool header.
static inline void **pool_allocator_from_header(__ds_pool_header_t *header) {
  return (void **) ++header;
  /* TODO: If the above doesn't work, use this.
   * return (void **) ( // Cast to return type
    ((char *) header)  // Cast to char * to move by bytes
    + sizeof(__ds_pool_header_t)); // Move over the header */
}

// Allocate a new block, initialize the free list inside and prepend
// the block to the old freelist head.
static inline void *create_new_block(
  size_t val_len,
  size_t new_block_index,
  __ds_pool_header_t *header
) {
  const size_t cells_per_block = (1 << DS_POOL_CELL_BITS);
  const size_t block_mem_len = cells_per_block * val_len;
  void *block = malloc(block_mem_len);

  for (size_t curr_cell_idx = 0; curr_cell_idx < cells_per_block; curr_cell_idx++) {
    void *curr_cell = ((char *) block) + val_len * curr_cell_idx;
    __ds_pool_cell_ref_t *curr_cell_ref = curr_cell;
    if (curr_cell_idx == cells_per_block - 1) {
      *curr_cell_ref = (__ds_pool_cell_ref_t){
        .block_idx = header->block_idx,
        .cell_idx = header->cell_idx,
      };
    } else {
      *curr_cell_ref = (__ds_pool_cell_ref_t){
        .block_idx = new_block_index,
        .cell_idx = curr_cell_idx + 1,
      };
    }
  }
  
  header->number_of_blocks++;
  header->block_idx = new_block_index;
  header->cell_idx = 0;

  return block;
}

/* INTERNAL */

void **__ds_new_pool_allocator(size_t val_len) {
  assert(DS_POOL_CELL_BITS + DS_POOL_BLOCK_BITS == 32);

  const size_t initial_blocks = 1;
  const size_t block_arr_mem_len = block_arr_memory_len(initial_blocks);
  __ds_pool_header_t *header = malloc(block_arr_mem_len);
  *header = (__ds_pool_header_t){
    .block_idx = NULL_CELL_REF.block_idx,
    .cell_idx = NULL_CELL_REF.cell_idx,
    .number_of_blocks = 0,
  };
  void **pool_allocator = pool_allocator_from_header(header);

  /* Initialize the individual blocks */ {
    void **curr_block = NULL;
    for (size_t curr_block_idx = 0; curr_block_idx < initial_blocks; curr_block_idx++) {
      curr_block = &pool_allocator[curr_block_idx];
      *curr_block = create_new_block(val_len, curr_block_idx, header);
    }
  }
  
  return pool_allocator;
}
