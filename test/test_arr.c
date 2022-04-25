#define MUNIT_ENABLE_ASSERT_ALIASES
#include "munit/munit.h"
#include "../arr/arr.h"

static MunitResult test_initialization(const MunitParameter params[], void* user_data_or_fixture) {
  /* Default initialization */ {
    #define LENGTH 100
    // Default initialization, all values zeroed
    arr_t(int) array_default = arr_create(int, LENGTH);

    // Length is properly set
    assert_int(arr_len(array_default), ==, LENGTH);

    for (int i = 0; i < arr_len(array_default); i++) {
      // All values are zeroed
      assert_int(array_default[i], ==, 0);
    }
  }

  /* Initialization with values */ {
    arr_t(char) array_values = arr_create(char, 4, {0, 1, 2, 3});

    // Length is properly set
    assert_int(arr_len(array_values), ==, 4);

    for (int i = 0; i < arr_len(array_values); i++) {
      // All values initialized properly
      assert_int(array_values[i], ==, i);
    }
  }

  return MUNIT_OK;
}

// Example function which uses array and its length
void double_array(arr_t(int) array) {
  for (int i = 0; i < arr_len(array); i++)
    array[i] *= 2;
}

static MunitResult test_array_parameter(const MunitParameter params[], void* user_data_or_fixture) {
  arr_t(int) array = arr_create(int, 8);
  for (int i = 0; i < arr_len(array); i++) {
    array[i] = i;
  }

  double_array(array);

  for (int i = 0; i < arr_len(array); i++) {
    assert_int(array[i], ==, 2*i);
  }
  
  return MUNIT_OK;
}

static MunitResult test_safe_access(const MunitParameter params[], void* user_data_or_fixture) {
  arr_t(int) array = arr_create(int, 8);
  for (int i = 0; i < arr_len(array); i++) {
    array[i] = i;
  }

  assert(arr_get(array, 1) == array[1]);
  assert(arr_get(array, 1) == 1);

  arr_set(array, 1, 123);

  assert(arr_get(array, 1) == 123);

  return MUNIT_OK;
}

static MunitTest tests[] = {
  {"/initialization", test_initialization, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL},
  {"/array_parameter", test_array_parameter, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL},
  {"/safe_access", test_safe_access, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL},
  {NULL, NULL, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL}
};

const MunitSuite arr_test_suite = {"/arr", tests, NULL, 1, MUNIT_SUITE_OPTION_NONE};

