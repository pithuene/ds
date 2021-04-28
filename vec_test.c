#include "vec.h"
#include <assert.h>
#include <stdio.h>

int main(void) {
  int * my_vec = vec(int, 5);
  assert(veclen(my_vec) == 0);

  int i;
  for (i = 0; i < 100; i++) {
    vecpush(my_vec, i);
  }
  assert(veclen(my_vec) == 100);

  assert(my_vec[1] == 1);
  my_vec[5] = 7;
  assert(my_vec[5] == 7);
  assert(vecpop(my_vec) == 99);
  assert(veclen(my_vec) == 99);

  int rmu = vecrmu(my_vec, 42);
  assert(rmu == 42);
  assert(veclen(my_vec) == 98);
  assert(my_vec[42] == 98);

  int rm = vecrm(my_vec, 42);
  assert(rm == 98);
  assert(veclen(my_vec) == 97);
  assert(my_vec[42] == 43);

  vecfree(my_vec);

  return 0;
}

