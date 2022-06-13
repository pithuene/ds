#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#define MUNIT_ENABLE_ASSERT_ALIASES
#include "munit/munit.h"
#include "../set/set.h"
#include "../set/set.c"

typedef struct {
  int key;
  char *value;
} Struct;

uint64_t struct_hash_function(void *key, size_t size) {
  return ((Struct *)key)->key;
}

bool struct_compare_function(void *key1, void *key2, size_t size) {
  return ((Struct *)key1)->key == ((Struct *)key2)->key;
}

static MunitResult test_add_remove(const MunitParameter params[], void* user_data_or_fixture) {
  set_t(Struct) set = set_create(Struct, 5, NULL, NULL);
  Struct new_element = {.key = 1, .value = "test"};
  set_add(set, new_element);

  assert(set_has(set, (Struct){.key = 1, .value = "test"}));

  set_remove(set, new_element);
  assert(!set_has(set, new_element));
  set_remove(set, new_element);
  assert(!set_has(set, new_element));

  for (int i = 0; i < 10; i++) {
    new_element.key = i + 10;
    set_add(set, new_element);
  }

  assert_uint(set_cap(set), ==, 16);

  set_free(set);

  return MUNIT_OK;
}

static MunitTest tests[] = {
  {"/add_remove", test_add_remove, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL},
  {NULL, NULL, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL}
};

const MunitSuite set_test_suite = {"/set", tests, NULL, 1, MUNIT_SUITE_OPTION_NONE};

