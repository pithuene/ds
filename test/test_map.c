#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#define MUNIT_ENABLE_ASSERT_ALIASES
#include "munit/munit.h"
#include "../map/map.h"
#include "../map/map.c"

static MunitResult test_ft_bitmap_size(const MunitParameter params[], void* user_data_or_fixture) {
  assert_uint(calculate_ft_bitmap_size(1), ==, 8);
  assert_uint(calculate_ft_bitmap_size(32), ==, 8);
  assert_uint(calculate_ft_bitmap_size(33), ==, 16);

  return MUNIT_OK;
}

static MunitResult test_ft_bitmap(const MunitParameter params[], void* user_data_or_fixture) {
  size_t memory_length = calculate_ft_bitmap_size(32);
  uint8_t *bitmap = calloc(1, memory_length);
  // 00 00 10 00 => Bucket 2 is full
  // 0  1  2  3
  *bitmap = 0x08;
  assert(!ft_is_full(bitmap, 1));
  assert(ft_is_full(bitmap, 2));
  assert(!ft_is_full(bitmap, 3));

  // 00 00 01 00 => Bucket 2 has a tombstone
  // 0  1  2  3
  *bitmap = 0x04;
  assert(!ft_has_tombstone(bitmap, 1));
  assert(ft_has_tombstone(bitmap, 2));
  assert(!ft_has_tombstone(bitmap, 3));

  ft_set_full(bitmap, 7, true);
  // 00 00 00 10
  // 4  5  6  7
  assert_uint8(bitmap[1], ==, 0x02);
  assert(ft_is_full(bitmap, 7));
  ft_set_full(bitmap, 7, false);
  assert_uint8(bitmap[1], ==, 0x00);
  assert(!ft_is_full(bitmap, 7));

  assert(!ft_has_tombstone(bitmap, 7));
  ft_set_tombstone(bitmap, 7, true);
  assert(ft_has_tombstone(bitmap, 7));
  assert_uint8(bitmap[1], ==, 0x01);
  ft_set_tombstone(bitmap, 7, false);
  assert(!ft_has_tombstone(bitmap, 7));

  // Idempotent set tombstone true
  ft_set_tombstone(bitmap, 5, true);
  ft_set_tombstone(bitmap, 5, true);
  assert(ft_has_tombstone(bitmap, 5));

  // Idempotent set tombstone false
  ft_set_tombstone(bitmap, 5, false);
  ft_set_tombstone(bitmap, 5, false);
  assert(!ft_has_tombstone(bitmap, 5));

  // Idempotent set full true
  ft_set_full(bitmap, 5, true);
  ft_set_full(bitmap, 5, true);
  assert(ft_is_full(bitmap, 5));

  // Idempotent set full false
  ft_set_full(bitmap, 5, false);
  ft_set_full(bitmap, 5, false);
  assert(!ft_is_full(bitmap, 5));

  return MUNIT_OK;
}

static MunitResult test_next_pow_2(const MunitParameter params[], void* user_data_or_fixture) {
  for (uint32_t i = 0; i < 1050; i++) {
    assert_uint32(calculate_next_pow_2(i), ==, pow(2, ceil(log(i)/log(2))));
  }

  return MUNIT_OK;
}

uint64_t inthash(int *num) {
  return *num;
}

bool intequals(int *a, int *b) {
  return *a == *b;
}

static MunitResult test_map(const MunitParameter params[], void* user_data_or_fixture) {
  map_t(int, int) map = map_create(int, int, 5, (ds_map_hash_func_t *) NULL, (ds_map_equals_func_t *) NULL);

  int key = 5;
  map_put(map, &key, 1234);
  assert_uint32(map_cap(map), ==, 8);
  map_reserve(map, 10);
  assert_uint32(map_cap(map), ==, 16);

  assert(map_has(map, &key));
  key = 6;
  assert(!map_has(map, &key));

  key = 5;
  assert_int(map_get(map, &key), ==, 1234);

  assert(map_remove(map, &key));
  assert(!map_remove(map, &key));
  assert(!map_has(map, &key));
  map_put(map, &key, 1234);
  assert(map_has(map, &key));
  assert_int(map_get(map, &key), ==, 1234);

  for (int i = 0; i < 98000; i++) {
    int key = i * i;
    map_put(map, &key, i*i);
  }

  for (int i = 0; i < 98000; i++) {
    int key = i * i;
    assert(map_has(map, &key));
    int value = map_get(map, &key);
    assert_int(value, ==, i*i);
  }

  map_free(map);

  return MUNIT_OK;
}

static MunitResult test_small_map(const MunitParameter params[], void* user_data_or_fixture) {
  map_t(int, int) map = map_create(int, int, 1, (ds_map_hash_func_t *) inthash, (ds_map_equals_func_t *) intequals);

  assert_uint32(map_cap(map), ==, 1);
  size_t map_capacity = map_cap(map);
  
  // Make sure the map doesn't grow
  map_put(map, &(int){0}, 0);
  assert_uint32(map_cap(map), ==, map_capacity);

  // Make sure the map does grow after another put
  map_put(map, &(int){1}, 1);
  assert_uint32(map_cap(map), >, map_capacity);

  map_free(map);

  return MUNIT_OK;
}

static MunitTest tests[] = {
  {"/ft_bitmap_size", test_ft_bitmap_size, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL},
  {"/ft_bitmap", test_ft_bitmap, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL},
  {"/next_pow_2", test_next_pow_2, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL},
  {"/map", test_map, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL},
  {"/small_map", test_small_map, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL},
  {NULL, NULL, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL}
};

const MunitSuite map_test_suite = {"/map", tests, NULL, 1, MUNIT_SUITE_OPTION_NONE};

