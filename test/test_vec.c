#include <stdint.h>
#define MUNIT_ENABLE_ASSERT_ALIASES
#include "munit/munit.h"
#include "../vec/vec.h"
#include "../vec/vec.c"

static MunitResult test_header_access(const MunitParameter params[], void* user_data_or_fixture) {
  vec_t(uint64_t) vector = vec_create(uint64_t, 10);
  __ds_vec_header_t *header = __ds_vec_header(vector);
  assert_uint(vec_len(vector), ==, 0);
  assert_uint(vec_cap(vector), ==, 10);
  void *returned_vector = vec_from_header(header);
  assert_ptr_equal(vector, returned_vector);
  return MUNIT_OK;
}

static MunitResult test_initialization(const MunitParameter params[], void* user_data_or_fixture) {
  vec_t(int) vector = vec_create(int, 10);

  assert_uint(vec_len(vector), ==, 0);
  assert_uint(vec_cap(vector), ==, 10);

  return MUNIT_OK;
}

static MunitResult test_grow(const MunitParameter params[], void* user_data_or_fixture) {
  vec_t(int) vector = vec_create(int, 10);

  assert_uint(vec_len(vector), ==, 0);
  assert_uint(vec_cap(vector), ==, 10);

  vec_grow(vector, 20);

  assert_uint(vec_len(vector), ==, 0);
  assert_uint(vec_cap(vector), ==, 20);

  vector[19] = 123; // Can access new slots

  return MUNIT_OK;
}

static MunitTest tests[] = {
  {"/header_access", test_header_access, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL},
  {"/initialization", test_initialization, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL},
  {"/grow", test_grow, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL},
  {NULL, NULL, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL}
};

const MunitSuite vec_test_suite = {"/vec", tests, NULL, 1, MUNIT_SUITE_OPTION_NONE};

