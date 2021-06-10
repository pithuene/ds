#include "linalloc.h"
#include "types.h"
#include <assert.h>
#include <stdio.h>

typedef struct {
  int i;
  char * s;
  char c;
} Struct;

int main(void) {
  int i; /* Once for stack and once for heap allocation */
  for (i = 0; i <= 1; i++) {
    NewLinAllocator(myalloc, 128, i);
    Struct * struc = linalloc(myalloc, sizeof(Struct));
    struc->i = 123451;
    struc->s = "Hello World";
    struc->c = 'a';

    int * integer = linalloc(myalloc, sizeof(int));
    *integer = 456;

    assert(struc->c == 'a');
    assert(*integer == 456);

    linfree(myalloc, integer);
    linfree(myalloc, struc);

    /* Only free the buffer if it was heap allocated */
    if (i == true) FreeLinAllocator(myalloc);
  }
  return 0;
}
