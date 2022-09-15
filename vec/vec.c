#include "vec.h"

#include <stdlib.h>
#include <assert.h>

/* NOT EXPOSED */

static inline size_t vec_memory_length(size_t cap, size_t val_len) {
  // cap and len are stored in a single uint64_t which requires an
  // alignment of 8 bytes, making padding unnecessary for any type parameter.
  return sizeof(uint64_t) + cap * val_len;
}

/* INTERNAL */

void *__ds_vec_create(size_t val_len, size_t cap) {
  const size_t memory_length = vec_memory_length(cap, val_len);
  __ds_vec_struct_t *vec = (__ds_vec_struct_t *) malloc(memory_length);
  vec->len = 0;
  vec->cap = cap;
  return &vec->items;
}

// Ensure the vector has space for a given number of elements.
// Reallocates the vector if necessary.
// Returns the vector since it may have moved.
void *__ds_vec_reserve_internal(
  void *vec_items, size_t new_cap, size_t val_len
) {
  __ds_vec_struct_t *vec = ds_container_of(vec_items, __ds_vec_struct_t, items);
  if (vec->cap >= new_cap) {
    // Capacity sufficient
    return &vec->items;
  }
  vec = realloc(vec, vec_memory_length(new_cap, val_len));
  vec->cap = new_cap;
  return &vec->items;
}

// Calculate the new capacity after another item is pushed.
size_t __ds_vec_capacity_after_push(void *vec_items) {
  __ds_vec_struct_t *vec = ds_container_of(vec_items, __ds_vec_struct_t, items);
  if (vec->len < vec->cap) {
    return vec->cap;
  } else if (vec->cap == 0) {
    return 1;
  } else {
    return vec->cap * 2;
  }
}

/* EXTERNAL */

void ds_vec_free(void *vec_items) {
  __ds_vec_struct_t *vec = ds_container_of(vec_items, __ds_vec_struct_t, items);
  free(vec);
}
