#include "map.h"
#include "../mem/arenaalloc.h"
#include <assert.h>
#include <stdio.h>

typedef struct {
  int number; 
  char * string;
} Struct;

int main(void) {
  map_t(Struct) my_map = map(Struct, 4);

  arena_allocator_t arena = new_arena_allocator(1024);

  /* Put */ {
    int i;
    char * str;
    Struct s;
    for (i = 0; i < 1024; i++) {
      str = arenaalloc(&arena, 8);
      sprintf(str, "key%d", i);
      Struct s;
      s.number = i;
      s.string = "Test123";
      mapput(my_map, str, s);
    }
  }

  /* Get */ {
    Struct s123 = mapget(my_map, "key123");
    assert(s123.number == 123);
    Struct s456 = mapget(my_map, "key456");
    assert(s456.number == 456);
  }

  mapfree(my_map);
  free_arena_allocator(&arena);
  return 0;
}

