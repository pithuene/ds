#include "linalloc.h"
#include <assert.h>
#include <string.h>
#include <stdio.h>

void test_alloc(linear_allocator_t * lin_allocator) {
  int * vals[32];
  int i;
  for (i = 0; i < 32; i++) {
    vals[i] = linalloc(lin_allocator, sizeof(int));
    *vals[i] = i*i;
  }
  for (i = 0; i < 32; i++) {
    assert(*vals[i] == i*i);
  }
  for (i = 31; i >= 0; i--) {
    linfree(lin_allocator, vals[i]);
  }
  char * str = linalloc(lin_allocator, sizeof(char) * 8);
  strcpy(str, "1234568");
  assert(strcmp(str, "1234567"));
}

int main(void) {
  void * buf1 = malloc(128);
  linear_allocator_t heapAlloc = new_linear_allocator(buf1);
  test_alloc(&heapAlloc);
  free(buf1);

  char buf2[128];
  linear_allocator_t stackAlloc = new_linear_allocator(buf2);
  test_alloc(&stackAlloc);

  return 0;
}
