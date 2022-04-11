#include "arr.h"
#include <assert.h>

// Example function which uses array and its length
void double_array(arr_t(int) array) {
  for (int i = 0; i < arr_len(array); i++)
    array[i] *= 2;
}

int main(void) {
  // Default initialization
  arr_t(int) array1 = arr_create(int, 8);
  for (int i = 0; i < arr_len(array1); i++) {
    array1[i] = i;
  }

  // Initialization with designated initializer
  arr_t(int) array2 = arr_create(int, 4, {0, 1, 2, 3});

  double_array(array1);
  double_array(array2);

  for (int i = 0; i < arr_len(array1); i++) {
    assert(array1[i] == 2*i);
  }
  for (int i = 0; i < arr_len(array2); i++) {
    assert(array2[i] == 2*i);
  }

  /* arr_get and arr_set */ {
    assert(arr_get(array1, 1) == 2);
    arr_set(array1, 1, 123);
    assert(arr_get(array1, 1) == 123);
  }

  return 0;
}
