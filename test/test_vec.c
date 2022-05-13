#define MUNIT_ENABLE_ASSERT_ALIASES
#include "munit/munit.h"
#include "../vec/vec.h"

static MunitResult test_initialization(const MunitParameter params[], void* user_data_or_fixture) {
  vec_t(int) vector = vec_create(int, 10);

  for (int i = 0; i < vec_len(vector); i++) {
    // Items zeroed initially
    assert_int(vector[i], ==, 0);
  }

  assert_uint(vec_len(vector), ==, 0);
  assert_uint(vec_cap(vector), ==, 10);

  return MUNIT_OK;
}

static MunitTest tests[] = {
  {"/initialization", test_initialization, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL},
  {NULL, NULL, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL}
};

const MunitSuite vec_test_suite = {"/vec", tests, NULL, 1, MUNIT_SUITE_OPTION_NONE};

