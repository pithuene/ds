#include "arenaalloc.h"
#include <assert.h>
#include <limits.h>

int main(void) {
  ArenaAllocator * arena = NewArenaAllocator(16);
  long * ptr1 = arenaalloc(arena, sizeof(long));
  long * ptr2 = arenaalloc(arena, sizeof(long));
  *ptr2 = 0;
  *ptr1 = LONG_MAX;
  assert(*ptr2 == 0);
  long * ptr3 = arenaalloc(arena, 24);
  assert(ptr3 == NULL);
  long * ptr4 = arenaalloc(arena, 16);
  *ptr4 = 128;
  arenafree(arena);
  return 0;
}
