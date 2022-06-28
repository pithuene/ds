#define MUNIT_ENABLE_ASSERT_ALIASES
#include "munit/munit.h"
#include "../mem/pool/pool.h"
#include <stdbool.h>
#include <math.h>

#include "../mem/pool/pool.c"

static MunitResult test_pow2(
  const MunitParameter params[], void *user_data_or_fixture
) {
#define TEST_POW(X) assert_int((int) (pow(2, X) + 0.5), ==, POW2(X))

  for (int i = 1; i < 16; i++) {
    TEST_POW(i);
  }

  return MUNIT_OK;
}

static MunitResult test_get_block_capacity(
  const MunitParameter params[], void *user_data_or_fixture
) {
  size_t expected[] = {2, 4, 8, 16, 32, 64, 128};
  for (int i = 0; i < (sizeof(expected) / sizeof(*expected)); i++) {
    assert_uint(get_block_capacity(i), ==, expected[i]);
  }

  return MUNIT_OK;
}

static MunitResult test_header_access(
  const MunitParameter params[], void *user_data_or_fixture
) {
  pool_allocator_t(uint64_t) allocator = pool_allocator_create(uint64_t);
  __ds_pool_header_t *header = header_from_pool_allocator((void **) allocator);
  assert_uint(header->block_idx, ==, 0);
  void **returned_allocator = pool_allocator_from_header(header);
  assert_ptr_equal(allocator, returned_allocator);
  pool_allocator_delete(allocator);
  return MUNIT_OK;
}

static MunitResult test_freelist_is_empty(
  const MunitParameter params[], void *user_data_or_fixture
) {
  __ds_pool_header_t header = {
    .block_idx = 0,
    .cell_idx = 0,
  };
  assert(!freelist_is_empty(&header));

  header.block_idx = MAX_BLOCKS - 1;
  assert(!freelist_is_empty(&header));

  header.block_idx--;
  header.cell_idx = MAX_CELLS_PER_BLOCK - 1;
  assert(!freelist_is_empty(&header));

  header.block_idx = MAX_BLOCKS - 1;
  assert(freelist_is_empty(&header));

  return MUNIT_OK;
}

static MunitResult test_is_cell_in_block(
  const MunitParameter params[], void *user_data_or_fixture
) {
  pool_allocator_t(uint64_t) allocator = pool_allocator_create(uint64_t);
  uint64_t *val = poolalloc(allocator);

  assert(is_cell_in_block(allocator[0], 0, val, sizeof(uint64_t)));
  assert(!is_cell_in_block(allocator[0], 0, val - 1, sizeof(uint64_t)));
  assert(!is_cell_in_block(allocator[0], 0, val - 2, sizeof(uint64_t)));

  pool_allocator_delete(allocator);

  return MUNIT_OK;
}

static MunitResult test_allocation(
  const MunitParameter params[], void *user_data_or_fixture
) {
  pool_allocator_t(uint64_t) palloc = pool_allocator_create(uint64_t);

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

  {
    // Free one value
    poolfree(palloc, vals[VAL_COUNT - 1]);
    // Allocate it again
    vals[VAL_COUNT - 1] = poolalloc(palloc);
    // If the freed cell was properly reused, no new block should have been
    // allocated.
    assert_uint64(header->number_of_blocks, ==, 6);
  }

  // Allocating one more slot will allocate a new block internally
  uint64_t *another_val = poolalloc(palloc);
  *another_val = 1234;
  assert_uint64(*another_val, ==, 1234);
  // Get the header pointer again since the realloc necessary for the creation
  // of another block may have invalidated the old one.
  header = header_from_pool_allocator((void **) palloc);
  assert_uint64(header->number_of_blocks, ==, 7);
  assert(!freelist_is_empty(header));

  __ds_pool_cell_ref_t head_before_free = freelist_head(header);
  // Free a value
  poolfree(palloc, another_val);
  // Freeing should have changed the freelist head
  assert(!cell_refs_equal(head_before_free, freelist_head(header)));
  // Allocate the same slot again
  uint64_t *another_val_again = poolalloc(palloc);
  // Free list should be the same again
  assert(cell_refs_equal(head_before_free, freelist_head(header)));
  // Same slot allocated
  assert_ptr_equal(another_val, another_val_again);

  /* Free an invalid pointer */ { assert(poolfree(palloc, NULL) == false); }

  pool_allocator_delete(palloc);

  return MUNIT_OK;
}

static MunitResult test_cellref_api(
  const MunitParameter params[], void *user_data_or_fixture
) {
  pool_allocator_t(uint64_t) palloc = pool_allocator_create(uint64_t);

  __ds_pool_cell_ref_t ref = poolalloc_cellref(palloc);
  assert_uint32(ref.block_idx, ==, 0);
  assert_uint32(ref.cell_idx, ==, 0);

  __ds_pool_header_t *header = header_from_pool_allocator((void **) palloc);
  assert_uint32(header->block_idx, ==, 0);
  assert_uint32(header->cell_idx, ==, 1);

  palloc[ref.block_idx][ref.cell_idx] = 1234;
  uint64_t *cell = &palloc[ref.block_idx][ref.cell_idx];

  poolfree_cellref(palloc, ref);

  assert_uint32(header->block_idx, ==, 0);
  assert_uint32(header->cell_idx, ==, 0);
  assert_uint32(header->number_of_blocks, ==, 1);

  uint64_t *allocated_normally = poolalloc(palloc);
  assert_ptr_equal(cell, allocated_normally);
  __ds_pool_cell_ref_t found_cellref = cellref_for_cell(
    (void **) palloc, allocated_normally, sizeof(*allocated_normally)
  );
  assert_uint32(found_cellref.block_idx, ==, 0);
  assert_uint32(found_cellref.cell_idx, ==, 0);

  assert_uint32(header->block_idx, ==, 0);
  assert_uint32(header->cell_idx, ==, 1);
  assert_uint32(header->number_of_blocks, ==, 1);

  *allocated_normally = 123;
  assert_uint64(
    palloc[found_cellref.block_idx][found_cellref.cell_idx], ==, 123
  );
  assert_uint64(*allocated_normally, ==, 123);

  poolfree(palloc, allocated_normally);

  return MUNIT_OK;
}

static MunitTest tests[] = {
  {"/pow2", test_pow2, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL},
  {"/get_block_capacity",
   test_get_block_capacity,
   NULL,
   NULL,
   MUNIT_TEST_OPTION_NONE,
   NULL},
  {"/header_access",
   test_header_access,
   NULL,
   NULL,
   MUNIT_TEST_OPTION_NONE,
   NULL},
  {"/freelist_is_empty",
   test_freelist_is_empty,
   NULL,
   NULL,
   MUNIT_TEST_OPTION_NONE,
   NULL},
  {"/is_cell_in_block",
   test_is_cell_in_block,
   NULL,
   NULL,
   MUNIT_TEST_OPTION_NONE,
   NULL},
  {"/allocation", test_allocation, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL},
  {"/cellref_api", test_cellref_api, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL},
  {NULL, NULL, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL}};

const MunitSuite mem_pool_test_suite = {
  "/mem/pool", tests, NULL, 1, MUNIT_SUITE_OPTION_NONE};
