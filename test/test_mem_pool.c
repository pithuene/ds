#define MUNIT_ENABLE_ASSERT_ALIASES
#include "munit/munit.h"
#include "../mem/pool/pool.h"
#include <stdbool.h>

__ds_pool_header_t *header_from_pool_allocator(void **allocator);

static bool freelist_is_empty(__ds_pool_header_t *header) {
  #define MAX_BLOCKS ((((unsigned int) 1) << DS_POOL_BLOCK_BITS))
  #define MAX_CELLS_PER_BLOCK ((((unsigned int) 1) << DS_POOL_CELL_BITS))
  return header->block_idx == MAX_BLOCKS - 1
    && header->cell_idx == MAX_CELLS_PER_BLOCK - 1;
}

static MunitResult test_allocation(const MunitParameter params[], void* user_data_or_fixture) {
  pool_allocator_t(uint64_t) palloc = new_pool_allocator(uint64_t);

  #define VAL_COUNT 126

  uint64_t *vals[VAL_COUNT];

  for (uint64_t i = 0; i < VAL_COUNT; i++) {
    vals[i] = poolalloc(palloc);
    *vals[i] = i;
  }

  for (uint64_t i = 0; i < VAL_COUNT; i++) {
    assert_uint64(*vals[i], ==, i);
  }

  __ds_pool_header_t *header = header_from_pool_allocator((void **) palloc);
  // After 126 insertions there are 6 blocks
  // 2 + 4 + 8 + 16 + 32 + 64 = 126
  assert_uint64(header->number_of_blocks, ==, 6);
  // These blocks are all completely filled, so the freelist is empty
  assert(freelist_is_empty(header));

  // Allocating one more slot will allocate a new block internally
  uint64_t *another_val = poolalloc(palloc);
  *another_val = 1234;
  assert_uint64(*another_val, ==, 1234);
  // Get the header pointer again since the realloc necessary for the creation
  // of another block may have invalidated the old one.
  header = header_from_pool_allocator((void **) palloc);
  assert_uint64(header->number_of_blocks, ==, 7);
  assert(!freelist_is_empty(header));

  return MUNIT_OK;
}

static MunitTest tests[] = {
  {"/allocation", test_allocation, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL},
  {NULL, NULL, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL}
};

const MunitSuite mem_pool_test_suite = {"/mem/pool", tests, NULL, 1, MUNIT_SUITE_OPTION_NONE};

