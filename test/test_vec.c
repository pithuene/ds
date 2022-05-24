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

  vec_reserve(vector, 200);

  assert_uint(vec_len(vector), ==, 0);
  assert_uint(vec_cap(vector), ==, 200);

  return MUNIT_OK;
}

static MunitResult test_empty_vec(const MunitParameter params[], void* user_data_or_fixture) {
  vec_t(int) vector = vec_create(int, 0);
  assert_uint(vec_len(vector), ==, 0);
  assert_uint(vec_cap(vector), ==, 0);
  vec_push(vector, 1234);
  assert_uint(vec_len(vector), ==, 1);
  assert_uint(vec_cap(vector), ==, 1);

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

static MunitResult test_safe_access(const MunitParameter params[], void* user_data_or_fixture) {
  vec_t(int) vector = vec_create(int, 8);
  for (int i = 0; i < vec_cap(vector); i++) {
    vec_push(vector, i);
  }
  assert_uint(vec_len(vector), ==, 8);

  assert(vec_get(vector, 1) == vector[1]);
  assert(vec_get(vector, 1) == 1);

  vec_set(vector, 1, 123);

  assert(vec_get(vector, 1) == 123);

  return MUNIT_OK;
}

static int int_compare(int *a, int *b) {
  return *a - *b;
}

static MunitResult test_qsort_bsearch(const MunitParameter params[], void* user_data_or_fixture) {
  vec_t(int) vector = vec_create(int, 8);

  int input[] = {4,2,3,1,8,5,11,6};
  for (int i = 0; i < 8; i++) {
    vec_push(vector, input[i]);
  }

  vec_qsort(vector, int_compare);

  int expected[] = {
    1,2,3,4,5,6,8,11
  };
  assert_memory_equal(vec_len(vector) * sizeof(*vector), vector, expected);

  int *found = vec_bsearch(vector, &(int){3}, int_compare);
  assert_not_null(found);
  assert_int(*found, ==, 3);

  int *not_found = vec_bsearch(vector, &(int){7}, int_compare);
  assert_null(not_found);

  return MUNIT_OK;
}

static MunitTest tests[] = {
  {"/header_access", test_header_access, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL},
  {"/initialization", test_initialization, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL},
  {"/reserve", test_reserve, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL},
  {"/push", test_push, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL},
  {"/empty_vec", test_empty_vec, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL},
  {"/safe_access", test_safe_access, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL},
  {"/qsort_bsearch", test_qsort_bsearch, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL},
  {NULL, NULL, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL}
};

const MunitSuite vec_test_suite = {"/vec", tests, NULL, 1, MUNIT_SUITE_OPTION_NONE};

