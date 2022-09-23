#include <stdint.h>
#include <stdio.h>
#define MUNIT_ENABLE_ASSERT_ALIASES
#include "../llist/llist.c"
#include "../llist/llist.h"
#include "munit/munit.h"

static MunitResult test_usage(
  const MunitParameter params[], void *user_data_or_fixture
) {
  llist_t(long) list = llist_create(long);

  llist_item_t(long) reference = NULL;

  // Append 1 - 50 and 52 - 99
  for (long i = 1; i < 100; i++) {
    llist_append(list, i);
    if (i == 50) {
      reference = llist_last(list);
      i++;  // Skip the 51st
    }
  }

  // Prepend the zero
  llist_prepend(list, 0);

  // Add the 51st after the 50th as a reference
  llist_append_after(list, reference, 51);

  long i = 0;
  for (llist_item_t(long) item = llist_first(list); item != NULL;
       item = llist_next(list, item)) {
    assert_long(*item, ==, i);
    i++;
  }
  assert_long(i, ==, 100);

  long sum = 0;
  for (llist_item_t(long) item = llist_first(list); item != NULL;
       item = llist_next(list, item)) {
    sum += *item;
  }
  assert_long(sum, ==, 4950);

  llist_free(list);

  llist_t(char *) string_list = llist_create(char *);
  llist_append(string_list, "Test1");
  llist_append(string_list, "Test2");
  llist_item_t(char *) j = llist_first(string_list);
  llist_item_t(char *) k = llist_next(string_list, j);
  *llist_next(string_list, j) = "Test3";
  assert_string_equal(*k, "Test3");
  llist_free(string_list);

  return MUNIT_OK;
}

static MunitResult test_empty_list(
  const MunitParameter params[], void *user_data_or_fixture
) {
  llist_t(long) list = llist_create(long);
  assert_null(llist_first(list));
  llist_free(list);
  return MUNIT_OK;
}

static MunitTest tests[] = {
  {"/usage", test_usage, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL},
  {"/empty_list", test_empty_list, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL},
  {NULL, NULL, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL}};

const MunitSuite llist_test_suite = {
  "/llist", tests, NULL, 1, MUNIT_SUITE_OPTION_NONE};
