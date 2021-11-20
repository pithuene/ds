#ifndef DS_LINALLOC_H
#define DS_LINALLOC_H

#include <stdlib.h>
#include <stdio.h>

typedef void * linear_allocator_t;

/* Initializes a new linear allocator using a given buffer */
linear_allocator_t new_linear_allocator(void * buffer);

/* Allocaltes SIZE bytes from a given linear allocator.
 * Does *not* check whether the underlying buffer has sufficient capacity */
void * linalloc(linear_allocator_t * allocator, size_t size);
/**
 * Freeing a pointer also frees all allocations made afterwards.
 * Usually, you only want to free the last allocation.
 * You don't need to free the individual allocations as long as the buffer is
 * freed.
 */
void linfree(linear_allocator_t * allocator, void * ptr);

#endif
