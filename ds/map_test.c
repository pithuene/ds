#include "map.h"
#include <assert.h>

typedef struct {
  int number; 
  char * string;
} Struct;

int main(void) {
  map_t(Struct) my_map = map(Struct, 4);

  /* Put */ {
    Struct s1;
    s1.number = 123;
    s1.string = "Some String";
    Struct s2;
    s2.number = 456;
    s2.string = "Some other String";
    mapput(my_map, "key1", s1);
    mapput(my_map, "key2", s2);
    mapput(my_map, "key3", s1);
    mapput(my_map, "key4", s1);
    mapput(my_map, "key5", s1);
  }

  /* Get */ {
    Struct s1 = mapget(my_map, "key5");
    assert(s1.number == 123);
    Struct s2 = mapget(my_map, "key2");
    assert(s2.number == 456);
  }

  mapfree(my_map);
  return 0;
}

