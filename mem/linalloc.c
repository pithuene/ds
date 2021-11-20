#include "linalloc.h"

/* Initializes a new linear allocator using a given buffer */
void * new_linear_allocator(void * buff) {
  return buff;
}

void * linalloc(void ** allocator, size_t size) {
  void * ptr = *allocator;
  *allocator = (char *) *allocator + size;
  return ptr;
}

void linfree(void ** allocator, void * ptr) {
  *allocator = ptr;
}

