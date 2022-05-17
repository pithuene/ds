#ifndef DS_VEC_H
#define DS_VEC_H

#include <stdint.h>
#include <stddef.h>
#include <assert.h>

#ifndef DS_NO_SHORT_NAMES
  #define vec_t       ds_vec_t
  #define vec_create  ds_vec_create
  #define vec_len     ds_vec_len
  #define vec_cap     ds_vec_cap
  #define vec_grow    ds_vec_grow
  #define vec_push    ds_vec_push
  #define vec_pop     ds_vec_pop
  #define vec_rm_swap ds_vec_rm_swap
  #define vec_rm      ds_vec_rm
  #define vec_free    ds_vec_free
#endif /* DS_NO_SHORT_NAMES */

/* INTERNAL */

typedef struct {
  uint64_t len : 32;
  uint64_t cap : 32;
} __ds_vec_header_t;

void *__ds_vec_create(size_t val_len, size_t cap);
__ds_vec_header_t *__ds_vec_header(void *vec);
void *__ds_vec_grow_internal(void *vec, size_t new_cap, size_t val_len);

/* EXTERNAL */

#define ds_vec_t(TYPE) TYPE *
// Allocate a new vector with a given initial capacity
#define ds_vec_create(TYPE, CAP) \
  ((ds_vec_t(TYPE)) __ds_vec_create(sizeof(TYPE), CAP))
size_t ds_vec_len(void *vec);
size_t ds_vec_cap(void *vec);
#define ds_vec_grow(VEC, NEWCAP) \
  ((VEC) = __ds_vec_grow_internal(VEC, NEWCAP, sizeof(*VEC)))
#define ds_vec_get(VEC, IDX) (assert(IDX >= 0), assert(IDX < ds_vec_len(VEC)), (VEC)[IDX])
#define ds_vec_set(VEC, IDX, VAL) (assert(IDX >= 0), assert(IDX < ds_vec_len(VEC)), (VEC)[IDX] = (VAL))

#endif /* DS_VEC_H */
