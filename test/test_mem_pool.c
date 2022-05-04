#define MUNIT_ENABLE_ASSERT_ALIASES
#include "munit/munit.h"
#include "../mem/pool/pool.h"

static MunitResult test_initialization(const MunitParameter params[], void* user_data_or_fixture) {
  pool_allocator_t(uint64_t) palloc = ds_new_pool_allocator(uint64_t);

  return MUNIT_OK;
}

static MunitTest tests[] = {
  {"/initialization", test_initialization, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL},
  {NULL, NULL, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL}
};

const MunitSuite mem_pool_test_suite = {"/mem/pool", tests, NULL, 1, MUNIT_SUITE_OPTION_NONE};

