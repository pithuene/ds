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

static MunitResult test_reserve(const MunitParameter params[], void* user_data_or_fixture) {
  vec_t(int) vector = vec_create(int, 10);

  assert_uint(vec_len(vector), ==, 0);
  assert_uint(vec_cap(vector), ==, 10);

  vec_reserve(vector, 20);

  assert_uint(vec_len(vector), ==, 0);
  assert_uint(vec_cap(vector), ==, 20);

  vector[19] = 123; // Can access new slots

  return MUNIT_OK;
}

static MunitResult test_push(const MunitParameter params[], void* user_data_or_fixture) {
  vec_t(int) vector = vec_create(int, 10);
  assert_uint(vec_len(vector), ==, 0);

  for (int i = 0; i < 10; i++) {
    vec_push(vector, i*i);
  }
  assert_uint(vec_len(vector), ==, 10);
  assert_uint(vec_cap(vector), ==, 10);
  for (int i = 0; i < 10; i++) {
    assert_int(vector[i], ==, i*i);
  }

  // Pushing one more value should double capacity
  vec_push(vector, 1234);
  assert_int(vector[10], ==, 1234);
  assert_uint(vec_len(vector), ==, 11);
  assert_uint(vec_cap(vector), ==, 20);

  return MUNIT_OK;
}

static MunitTest tests[] = {
  {"/header_access", test_header_access, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL},
  {"/initialization", test_initialization, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL},
  {"/reserve", test_reserve, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL},
  {"/push", test_push, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL},
  {NULL, NULL, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL}
};

const MunitSuite vec_test_suite = {"/vec", tests, NULL, 1, MUNIT_SUITE_OPTION_NONE};

