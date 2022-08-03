#include "munit/munit.h"

extern const MunitSuite arr_test_suite;
extern const MunitSuite vec_test_suite;
extern const MunitSuite heap_test_suite;
extern const MunitSuite map_test_suite;
extern const MunitSuite set_test_suite;
extern const MunitSuite mem_pool_test_suite;
extern const MunitSuite util_test_suite;

int main(int argc, const char *argv[]) {
  MunitSuite suites[] = {
    arr_test_suite,
    vec_test_suite,
    heap_test_suite,
    map_test_suite,
    set_test_suite,
    mem_pool_test_suite,
    util_test_suite,
    {NULL, NULL, NULL, 0, MUNIT_SUITE_OPTION_NONE}};

  const MunitSuite top_level_suite = {
    "/ds", /* name */
    (MunitTest[]
    ){{NULL, NULL, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL}}, /* tests */
    suites,                                                    /* suites */
    1,                                                         /* iterations */
    MUNIT_SUITE_OPTION_NONE                                    /* options */
  };
  return munit_suite_main(&top_level_suite, NULL, argc, (char *const *) argv);
}
