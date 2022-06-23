#include "vec.h"

#include <stdlib.h>
#include <assert.h>

/* NOT EXPOSED */

static inline size_t vec_memory_length(size_t cap, size_t val_len) {
  // The __ds_vec_header_t requires an alignment of 8 bytes,
  // making padding unnecessary for any type parameter.
  return sizeof(__ds_vec_header_t) + cap * val_len;
}

static inline void *vec_from_header(__ds_vec_header_t *header) {
  return (void *) (header + 1);
}

/* INTERNAL */

void *__ds_vec_create(size_t val_len, size_t cap) {
  const size_t memory_length = vec_memory_length(cap, val_len);
  __ds_vec_header_t *header = (__ds_vec_header_t *) malloc(memory_length);
  *header = (__ds_vec_header_t){
    .len = 0,
    .cap = cap,
  };
  return vec_from_header(header);
}

__ds_vec_header_t *__ds_vec_header(void *vec) {
  void *header = ((char *) vec) - sizeof(__ds_vec_header_t);
  return (__ds_vec_header_t *) header;
}

// Ensure the vector has space for a given number of elements.
// Reallocates the vector if necessary.
// Returns the vector since it may have moved.
void *__ds_vec_reserve_internal(void *vec, size_t new_cap, size_t val_len) {
  __ds_vec_header_t *header = __ds_vec_header(vec);
  if (header->cap >= new_cap) {
    // Capacity sufficient
    return vec;
  }
  header =
    (__ds_vec_header_t *) realloc(header, vec_memory_length(new_cap, val_len));
  header->cap = new_cap;
  return vec_from_header(header);
}

// Calculate the new capacity after another item is pushed.
size_t __ds_vec_capacity_after_push(void *vec) {
  __ds_vec_header_t *header = __ds_vec_header(vec);
  if (header->len < header->cap) {
    return header->cap;
  } else if (header->cap == 0) {
    return 1;
  } else {
    return header->cap * 2;
  }
}

/* EXTERNAL */

size_t ds_vec_len(void *vec) { return __ds_vec_header(vec)->len; }

size_t ds_vec_cap(void *vec) { return __ds_vec_header(vec)->cap; }
