#include <stdint.h>
#include <stdio.h>
#define MUNIT_ENABLE_ASSERT_ALIASES
#include "munit/munit.h"
#include "../llist/llist.h"
#include "../llist/llist.c"

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

  llist_free(list);
  return MUNIT_OK;
}

static MunitTest tests[] = {
  {"/usage", test_usage, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL},
  {NULL, NULL, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL}};

const MunitSuite llist_test_suite = {
  "/llist", tests, NULL, 1, MUNIT_SUITE_OPTION_NONE};
