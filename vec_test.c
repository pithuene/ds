#include "vec.h"
#include <assert.h>
#include <stdio.h>

int main(void) {
  /* Initialize */
  int *my_vec = vec(int, 2);
  assert(veclen(my_vec) == 0);
  assert(veclen(my_vec) == 0);

  /* Add elements */ {
    int i;
    for (i = 0; i < 100; i++) {
      vecpush(my_vec, i);
    }
    assert(veclen(my_vec) == 100);
    printf("Cap is: %lu\n", veccap(my_vec));
    assert(veccap(my_vec) == 128);
  }

  /* Set and get values */
  assert(my_vec[1] == 1);
  my_vec[5] = 7;
  assert(my_vec[5] == 7);

  /* Pop element */
  assert(vecpop(my_vec) == 99);
  assert(veclen(my_vec) == 99);
  assert(veccap(my_vec) == 128);

  /* Unordered Remove */ {
    int rmu = vecrmu(my_vec, 42);
    assert(rmu == 42);
    assert(veclen(my_vec) == 98);
    assert(my_vec[42] == 98);
    assert(veccap(my_vec) == 128);
  }


  /* Ordered Remove */ {
    int rm = vecrm(my_vec, 42);
    assert(rm == 98);
    assert(veclen(my_vec) == 97);
    assert(my_vec[42] == 43);
    assert(veccap(my_vec) == 128);
  }

  /* Cleanup */
  vecfree(my_vec);
  return 0;
}
