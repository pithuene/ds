#include <stdint.h>
#define MUNIT_ENABLE_ASSERT_ALIASES
#include "munit/munit.h"
#include "../util/container_of.h"

static MunitResult test_container_of(
  const MunitParameter params[], void *user_data_or_fixture
) {
  typedef struct {
    int a;
    int b;
    int c;
  } Struct;
  Struct s;
  int *b_ptr = &s.b;

  Struct *s_ptr = container_of(b_ptr, Struct, b);

  assert_ptr_equal(s_ptr, &s);
  return MUNIT_OK;
}

static MunitTest tests[] = {
  {"/container_of",
   test_container_of,
   NULL,
   NULL,
   MUNIT_TEST_OPTION_NONE,
   NULL},
  {NULL, NULL, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL}};

const MunitSuite util_test_suite = {
  "/util", tests, NULL, 1, MUNIT_SUITE_OPTION_NONE};
