#include "pool.h"
#include <stdlib.h>
#include <stdbool.h>
#include <assert.h>

/* NOT EXPOSED */

#define MAX_BLOCKS ((((unsigned int) 1) << DS_POOL_BLOCK_BITS))
#define MAX_CELLS_PER_BLOCK ((((unsigned int) 1) << DS_POOL_CELL_BITS))

static const __ds_pool_cell_ref_t NULL_CELL_REF = {
  .block_idx = MAX_BLOCKS - 1,
  .cell_idx  = MAX_CELLS_PER_BLOCK - 1,
};

static inline size_t block_arr_memory_len(size_t number_of_blocks) {
  return sizeof(__ds_pool_header_t) + sizeof(void *) * number_of_blocks;
}

// Returns a pointer to the first block pointer for a given pool header.
static inline void **pool_allocator_from_header(__ds_pool_header_t *header) {
  return (void **) (header + 1);
  /* TODO: If the above doesn't work, use this.
   * return (void **) ( // Cast to return type
    ((char *) header)  // Cast to char * to move by bytes
    + sizeof(__ds_pool_header_t)); // Move over the header */
}

static inline __ds_pool_header_t *header_from_pool_allocator(void **allocator) {
  return ((__ds_pool_header_t *) allocator) - 1;
}

static inline void *get_allocator_block(void **allocator, uint32_t block_idx) {
  return allocator[block_idx];
}

static inline __ds_pool_cell_ref_t *get_block_cell(void *block, uint32_t cell_idx, size_t val_len) {
  return (__ds_pool_cell_ref_t *) (((char *) block) + val_len * cell_idx);
}

static inline bool freelist_is_empty(__ds_pool_header_t *header) {
  return header->block_idx == MAX_BLOCKS - 1
      && header->cell_idx == MAX_CELLS_PER_BLOCK - 1;
}

// Allocate a new block, initialize the free list inside and prepend
// the block to the old freelist head.
// The new block still needs to be added to the blocks array.
static inline void *create_new_block(
  size_t val_len, // Length of one of the generic values
  size_t new_block_index, // The index the new block will have
  __ds_pool_header_t *header // Reference to the pool header, so the freelist head can be read and updated.
) {
  const size_t cells_per_block = 1 << header->next_block_capacity_power;
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
  
  header->block_idx = new_block_index;
  header->cell_idx = 0;
  header->number_of_blocks++;
  header->next_block_capacity_power++;
  if (header->next_block_capacity_power > DS_POOL_CELL_BITS) {
    header->next_block_capacity_power = DS_POOL_CELL_BITS;
  }

  return block;
}

// Append a given block to the end of the block array.
// Returns the pointer to the allocator as this may have changed
// through realloc.
static inline void **append_new_block(__ds_pool_header_t *header, void *new_block) {
  size_t block_arr_mem_len = block_arr_memory_len(header->number_of_blocks);
  header = realloc((void *) header, block_arr_mem_len);
  void **new_allocator = pool_allocator_from_header(header);
  new_allocator[header->number_of_blocks - 1] = new_block;
  return new_allocator;
}

/* INTERNAL */

void **__ds_new_pool_allocator(size_t val_len) {
  assert(DS_POOL_CELL_BITS + DS_POOL_BLOCK_BITS == 32);
  assert((1 << DS_POOL_BLOCK_CAP_POW_BITS) - 1 >= DS_POOL_CELL_BITS);

  const size_t initial_blocks = 1;
  const size_t block_arr_mem_len = block_arr_memory_len(initial_blocks);
  __ds_pool_header_t *header = malloc(block_arr_mem_len);
  *header = (__ds_pool_header_t){
    .block_idx = NULL_CELL_REF.block_idx,
    .cell_idx = NULL_CELL_REF.cell_idx,
    .number_of_blocks = 0,
    .next_block_capacity_power = 1,
  };
  void **pool_allocator = pool_allocator_from_header(header);

  /* Initialize the individual blocks */ {
    void **curr_block = NULL;
    for (size_t curr_block_idx = 0; curr_block_idx < initial_blocks; curr_block_idx++) {
      curr_block = pool_allocator + curr_block_idx;
      *curr_block = create_new_block(val_len, curr_block_idx, header);
    }
  }
  
  return pool_allocator;
}

void **__ds_pool_ensure_free_cell_internal(void **allocator, size_t val_len) {
  __ds_pool_header_t *header = header_from_pool_allocator(allocator);
  if (
    header->block_idx == NULL_CELL_REF.block_idx &&
    header->cell_idx == NULL_CELL_REF.cell_idx
  ) {
    // Freelist is empty, there are no free cells left.
    void *new_block = create_new_block(val_len, header->number_of_blocks, header);
    allocator = append_new_block(header, new_block);
  }
  return allocator;
}

// Get the freelist head block index
uint32_t __ds_poolalloc_head_block_idx(void **allocator) {
  __ds_pool_header_t *header = header_from_pool_allocator(allocator);
  return header->block_idx;
}

// Get the freelist head cell index
// **and remove the head from the freelist**.
uint32_t __ds_poolalloc_head_cell_idx(void **allocator, size_t val_len) {
  __ds_pool_header_t *header = header_from_pool_allocator(allocator);

  void *head_block = get_allocator_block(allocator, header->block_idx);
  __ds_pool_cell_ref_t *head_cell = get_block_cell(head_block, header->cell_idx, val_len);

  uint32_t old_head_cell_idx = header->cell_idx;

  // Move freelist head to the next entry
  header->block_idx = head_cell->block_idx;
  header->cell_idx = head_cell->cell_idx;

  return old_head_cell_idx;
}
