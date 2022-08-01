#include "vec.h"

#include <stdlib.h>
#include <assert.h>

/* NOT EXPOSED */

static inline size_t vec_memory_length(size_t cap, size_t val_len) {
  // The __ds_vec_header_t requires an alignment of 8 bytes,
  // making padding unnecessary for any type parameter.
  return sizeof(__ds_vec_header_t) + cap * val_len;
}

/* INTERNAL */

void *__ds_vec_create(size_t val_len, size_t cap) {
  const size_t memory_length = vec_memory_length(cap, val_len);
  __ds_vec_struct_t *vec = (__ds_vec_struct_t *) malloc(memory_length);
  vec->header = (__ds_vec_header_t){
    .len = 0,
    .cap = cap,
  };
  return &vec->items;
}

// Ensure the vector has space for a given number of elements.
// Reallocates the vector if necessary.
// Returns the vector since it may have moved.
void *__ds_vec_reserve_internal(
  void *vec_items, size_t new_cap, size_t val_len
) {
  __ds_vec_struct_t *vec = ds_container_of(vec_items, __ds_vec_struct_t, items);
  if (vec->header.cap >= new_cap) {
    // Capacity sufficient
    return &vec->items;
  }
  vec = (__ds_vec_struct_t *) realloc(vec, vec_memory_length(new_cap, val_len));
  vec->header.cap = new_cap;
  return &vec->items;
}

// Calculate the new capacity after another item is pushed.
size_t __ds_vec_capacity_after_push(void *vec_items) {
  __ds_vec_struct_t *vec = ds_container_of(vec_items, __ds_vec_struct_t, items);
  if (vec->header.len < vec->header.cap) {
    return vec->header.cap;
  } else if (vec->header.cap == 0) {
    return 1;
  } else {
    return vec->header.cap * 2;
  }
}
