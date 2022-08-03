#include <stdint.h>
#include <stdio.h>
#define MUNIT_ENABLE_ASSERT_ALIASES
#include "munit/munit.h"
#include "../heap/heap.h"
#include "../heap/heap.c"

bool int_is_lower_than(void *a, void *b) { return *(int *) a < *(int *) b; }

static MunitResult test_all(
  const MunitParameter params[], void *user_data_or_fixture
) {
  heap_t(int) heap = heap_create(int, int_is_lower_than);

  assert_true(heap_is_empty(heap));

  heap_insert(heap, 7);
  assert_false(heap_is_empty(heap));
  heap_insert(heap, 10);
  heap_insert(heap, 3);
  heap_insert(heap, 5);
  heap_insert(heap, 8);
  heap_insert(heap, 2);

  assert_int(heap_peek(heap), ==, 10);

  assert_false(heap_is_empty(heap));

  assert_int(heap_pop(heap), ==, 10);
  assert_int(heap_pop(heap), ==, 8);
  assert_int(heap_pop(heap), ==, 7);
  assert_int(heap_pop(heap), ==, 5);
  assert_int(heap_pop(heap), ==, 3);
  assert_int(heap_pop(heap), ==, 2);
  assert_true(heap_is_empty(heap));

  {
    for (int i = 0; i < 8; i++) {
      heap_insert(heap, i);
    }

    // If the temp slot at the end of the vector isn't kept clear,
    // this pops 7 twice and is empty after popping 1.

    assert_int(heap_pop(heap), ==, 7);
    assert_int(heap_pop(heap), ==, 6);
    assert_int(heap_pop(heap), ==, 5);
    assert_int(heap_pop(heap), ==, 4);
    assert_int(heap_pop(heap), ==, 3);
    assert_int(heap_pop(heap), ==, 2);
    assert_int(heap_pop(heap), ==, 1);
    assert_int(heap_pop(heap), ==, 0);
    assert_true(ds_heap_is_empty(heap));
  }

  heap_free(heap);
  return MUNIT_OK;
}

typedef struct {
  char *name;
  long distance;
  int population;
  char postcode[5];
} city_t;

bool city_is_lower_than(void *a, void *b) {
  return ((city_t *) a)->distance > ((city_t *) b)->distance;
}

static MunitResult test_with_struct(
  const MunitParameter params[], void *user_data_or_fixture
) {
  heap_t(city_t) cities = heap_create(city_t, city_is_lower_than);

  heap_insert(
    cities,
    (city_t){
      .name = "Berlin",
      .distance = 235,
      .population = 3000000,
      .postcode = {1, 2, 3, 4, 5},
    }
  );

  heap_insert(
    cities,
    (city_t){
      .name = "Munich",
      .distance = 487,
      .population = 2000000,
      .postcode = {6, 7, 8, 9, 7},
    }
  );

  heap_insert(
    cities,
    (city_t){
      .name = "Hanover",
      .distance = 9,
      .population = 600000,
      .postcode = {3, 0, 4, 9, 9},
    }
  );

  city_t nearest_city = ds_heap_pop(cities);

  assert_string_equal(nearest_city.name, "Hanover");

  return MUNIT_OK;
}

static MunitTest tests[] = {
  {"/all", test_all, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL},
  {"/with_struct", test_with_struct, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL},
  {NULL, NULL, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL}};

const MunitSuite heap_test_suite = {
  "/heap", tests, NULL, 1, MUNIT_SUITE_OPTION_NONE};
