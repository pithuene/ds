#ifndef DS_LINALLOC_H
#define DS_LINALLOC_H

#include <stdlib.h>
#include <stdio.h>

typedef enum {
  LinAllocStatic = 0,
  LinAllocDynamic = 1
} LinAllocatorType;

/**
 * Initializes a new linear allocator with name NAME and a capacity of CAP bytes.
 * If DYNAMIC is true, the buffer is heap allocated, otherwise it is stack allocated.
 */
#define NewLinAllocator(NAME, CAP, DYNAMIC) \
  void * NAME; \
  {\
    if (DYNAMIC == LinAllocStatic) { \
      char lin_alloc_buf[CAP]; \
      NAME = &lin_alloc_buf; \
    } else { \
      NAME = malloc(CAP); \
    } \
  }

/**
 * Frees a linear allocator.
 * Only makes sense for heap allocated buffers
 */
#define FreeLinAllocator(ALLOC) (ds_FreeLinAllocator(&ALLOC))

void ds_FreeLinAllocator(void ** allocator) {
  free(*allocator);
  *allocator = NULL;
}

/**
 * Allocaltes SIZE bytes from a given linear allocator.
 * Does *not* check whether the underlying buffer has sufficient capacity
 */
#define linalloc(ALLOC, SIZE) (ds_linalloc(&ALLOC, SIZE))

void * ds_linalloc(void ** allocator, size_t size) {
  void * ptr = *allocator;
  *allocator = (char *) *allocator + size;
  return ptr;
}

/**
 * Freeing a pointer also frees all allocations made afterwards.
 * Usually, you only want to free the last allocation.
 */
#define linfree(ALLOC, PTR) (ds_linfree(&ALLOC, PTR))

void ds_linfree(void ** allocator, void * ptr) {
  *allocator = ptr;
}

#endif
