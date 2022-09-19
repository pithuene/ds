#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#define MUNIT_ENABLE_ASSERT_ALIASES
#include "munit/munit.h"
#include "../set/set.h"
#include "../set/set.c"

#include "../vec/vec.h"

typedef struct {
  int key;
  char *value;
} Struct;

uint64_t struct_hash_function(void *key, size_t size) {
  return ((Struct *) key)->key;
}

bool struct_compare_function(void *key1, void *key2, size_t size) {
  return ((Struct *) key1)->key == ((Struct *) key2)->key;
}

static MunitResult test_mod_pow2(
  const MunitParameter params[], void *user_data_or_fixture
) {
  for (uint32_t p = 0; p < 10; p++) {
    uint32_t two_to_the_p = 1 << p;
    if (p == 3) assert_uint32(two_to_the_p, ==, 8);
    for (uint32_t x = 0; x < 1000000; x += 1333) {
      assert_uint32(mod_pow2(x, two_to_the_p), ==, x % two_to_the_p);
    }
  }
  return MUNIT_OK;
}

static MunitResult test_add_remove(
  const MunitParameter params[], void *user_data_or_fixture
) {
  set_t(Struct) set = set_create(Struct, 5, NULL, NULL);
  Struct new_element = {.key = 1, .value = "test"};
  set_add(set, new_element);

  assert(set_has(set, (Struct){.key = 1, .value = "test"}));

  set_remove(set, new_element);
  assert(!set_has(set, new_element));
  set_remove(set, new_element);
  assert(!set_has(set, new_element));

  // Make sure getting a non existent entry crashes.
  // There used to be an issue, where the increment used to skip the temp slot
  // resulted in an overflow of the invalid index value, which made this return
  // the temp slot value instead of crash.
  assert_uint32(
    __ds_set_get_internal(set, &(set)[0], sizeof(*set)) + 1, >, 100000
  );

  for (int i = 0; i < 10; i++) {
    new_element.key = i + 10;
    set_add(set, new_element);
  }

  assert_uint(set_cap(set), ==, 16);

  set_free(set);

  return MUNIT_OK;
}

int int_compare(const void *a, const void *b) {
  return *((int *) a) - *((int *) b);
}

static MunitResult test_foreach(
  const MunitParameter params[], void *user_data_or_fixture
) {
#define MAX_CHECKED 100

  set_t(int) set = set_create(int, 20, NULL, NULL);
  vec_t(int) entries_vec = vec_create(int, 20);

  for (int i = 0; i < MAX_CHECKED; i++) {
    set_add(set, i * i);
    vec_push(entries_vec, i * i);
  }

  assert_int(set_cap(set), >=, MAX_CHECKED);
  assert_int(set_size(set), ==, MAX_CHECKED);

  assert_false(set_has(set, -1));
  for (int i = 0; i < MAX_CHECKED; i++) {
    assert_true(set_has(set, i * i));
    assert_int(set_get(set, i * i), ==, i * i);
  }
  assert_false(set_has(set, MAX_CHECKED * MAX_CHECKED));

  set_foreach(int x, set) {
    int *res = vec_bsearch(entries_vec, &x, int_compare);
    assert_ptr_not_null(res);
    assert_int(*res, ==, x);
  }

  /* Nested foreach */ {
    set_foreach(int x, set) {
      bool found = false;
      set_foreach(int y, set) {
        if (y == x) found = true;
      }
      assert_true(found);
    }
  }

  set_free(set);

  return MUNIT_OK;
}

static MunitTest tests[] = {
  {"/mod_pow2", test_mod_pow2, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL},
  {"/add_remove", test_add_remove, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL},
  {"/foreach", test_foreach, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL},
  {NULL, NULL, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL}};

const MunitSuite set_test_suite = {
  "/set", tests, NULL, 1, MUNIT_SUITE_OPTION_NONE};
