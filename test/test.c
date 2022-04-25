#include "munit/munit.h"

extern const MunitSuite arr_test_suite;

int main (int argc, const char* argv[]) {
  MunitSuite suites[] = {
    arr_test_suite,
    { NULL, NULL, NULL, 0, MUNIT_SUITE_OPTION_NONE }
  };

  const MunitSuite top_level_suite = {
    "/ds", /* name */
    (MunitTest[]){
      { NULL, NULL, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL }
    }, /* tests */
    suites, /* suites */
    1, /* iterations */
    MUNIT_SUITE_OPTION_NONE /* options */
  };
  return munit_suite_main(&top_level_suite, NULL, argc, (char * const*) argv);
}
