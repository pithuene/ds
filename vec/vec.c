#include "vec.h"

#include <stdlib.h>

/* INTERNAL */

void *__ds_vec_create(size_t val_len, size_t cap) {
  // The __ds_vec_header_t requires an alignment of 8 bytes,
  // making padding unnecessary for any type parameter.
  const size_t memory_length = sizeof(__ds_vec_header_t) + cap * val_len;
  __ds_vec_header_t *header = (__ds_vec_header_t *) calloc(1, memory_length);
  *header = (__ds_vec_header_t){
    .len = 0,
    .cap = cap,
  };
  void *first_item = ((char *) header) + sizeof(__ds_vec_header_t);
  return first_item;
}

__ds_vec_header_t *__ds_vec_header(void *vec) {
  void *header = ((char *) vec) - sizeof(__ds_vec_header_t);
  return (__ds_vec_header_t *) header;
}

/* EXTERNAL */

size_t ds_vec_len(void *vec) {
  return __ds_vec_header(vec)->len;
}

size_t ds_vec_cap(void *vec) {
  return __ds_vec_header(vec)->cap;
}
