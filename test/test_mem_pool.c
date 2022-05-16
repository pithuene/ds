#define MUNIT_ENABLE_ASSERT_ALIASES
#include "munit/munit.h"
#include "../mem/pool/pool.h"
#include <stdbool.h>
#include <math.h>

#include "../mem/pool/pool.c"

static MunitResult test_pow2(const MunitParameter params[], void* user_data_or_fixture) {
  #define POW2(X) (1 << X)
  #define TEST_POW(X) assert_int((int)(pow(2, X) + 0.5), ==, POW2(X))

  for (int i = 1; i < 16; i++) {
    TEST_POW(i);
  }

  return MUNIT_OK;
}

static MunitResult test_header_access(const MunitParameter params[], void* user_data_or_fixture) {
  pool_allocator_t(uint64_t) allocator = new_pool_allocator(uint64_t);
  __ds_pool_header_t *header = header_from_pool_allocator((void **) allocator);
  assert_uint(header->block_idx, ==, 0);
  void **returned_allocator = pool_allocator_from_header(header);
  assert_ptr_equal(allocator, returned_allocator);
  return MUNIT_OK;
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
  {"/pow2", test_pow2, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL},
  {"/header_access", test_header_access, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL},
  {"/allocation", test_allocation, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL},
  {NULL, NULL, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL}
};

const MunitSuite mem_pool_test_suite = {"/mem/pool", tests, NULL, 1, MUNIT_SUITE_OPTION_NONE};

