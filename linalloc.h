#ifndef DS_LINALLOC_H
#define DS_LINALLOC_H

#include <stdlib.h>
#include <stdio.h>

/* Initializes a new linear allocator using a given buffer */
void * NewLinAllocator(void * buff) {
  return buff;
}


/* Allocaltes SIZE bytes from a given linear allocator.
 * Does *not* check whether the underlying buffer has sufficient capacity */
#define linalloc(ALLOC, SIZE) (ds_linalloc(&ALLOC, SIZE))

void * ds_linalloc(void ** allocator, size_t size) {
  void * ptr = *allocator;
  *allocator = (char *) *allocator + size;
  return ptr;
}

/**
 * Freeing a pointer also frees all allocations made afterwards.
 * Usually, you only want to free the last allocation.
 * You don't need to free the individual allocations as long as the buffer is
 * freed.
 */
#define linfree(ALLOC, PTR) (ds_linfree(&ALLOC, PTR))

void ds_linfree(void ** allocator, void * ptr) {
  *allocator = ptr;
}

#endif
