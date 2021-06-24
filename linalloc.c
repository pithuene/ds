#include "linalloc.h"

/* Initializes a new linear allocator using a given buffer */
void * newLinAllocator(void * buff) {
  return buff;
}

void * ds_linalloc(void ** allocator, size_t size) {
  void * ptr = *allocator;
  *allocator = (char *) *allocator + size;
  return ptr;
}

void ds_linfree(void ** allocator, void * ptr) {
  *allocator = ptr;
}

