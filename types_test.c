#include "types.h"
#include <assert.h>
#include <stdio.h>

int main(void) {
  bool x = false;
  assert(x != true);
  assert(x == false);

  /* Check that integer sizes are as expected */
  /* This is not spec defined */ 
  /* On some systems int may be 2 bytes or long 4 bytes */ 
  assert(sizeof(u8) == 1);
  assert(sizeof(i8) == 1);

  assert(sizeof(u16) == 2);
  assert(sizeof(i16) == 2);

  assert(sizeof(u32) == 4);
  assert(sizeof(i32) == 4);

  assert(sizeof(u64) == 8);
  assert(sizeof(i64) == 8);

  assert(sizeof(f32) == 4);
  assert(sizeof(f64) == 8);
  assert(sizeof(f128) == 16);

  return 0;
}
