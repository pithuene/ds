#include "poolalloc.h"
#include <assert.h>
#include <limits.h>

typedef struct {
  int number; 
  char c;
  void * ptr;
} Struct;

int main(void) {
  PoolAllocator pool = newPoolAllocator(sizeof(Struct));

  int num = 10000;

  Struct * strcts[num];

  int i;
  for (i = 0; i < num; i++) {
    strcts[i] = poolalloc(&pool);
    strcts[i]->number = i;
    strcts[i]->c = i;
  }

  for (i = 0; i < num; i++) {
    assert(strcts[i]->number == i);
    poolfree(&pool, strcts[i]);
  }

  deletePoolAllocator(&pool);
  return 0;
}
