#include "arenaalloc.h"
#include <assert.h>
#include <limits.h>

int main(void) {
  arena_allocator_t arena = new_arena_allocator(16);
  long * ptr1 = arenaalloc(&arena, sizeof(long));
  long * ptr2 = arenaalloc(&arena, sizeof(long));
  *ptr2 = 0;
  *ptr1 = LONG_MAX;
  assert(*ptr2 == 0);
  long * ptr3 = arenaalloc(&arena, 24);
  assert(ptr3 == NULL);
  long * ptr4 = arenaalloc(&arena, 16);
  *ptr4 = 128;

  arena_allocator_reset(&arena);
  assert(veclen(arena.containers) == 0);

  free_arena_allocator(&arena);
  return 0;
}
