#ifndef DS_VEC_H
#define DS_VEC_H

#include <stdint.h>
#include <stddef.h>
#include <stdlib.h>
#include <assert.h>

#include "../util.h"

#ifndef DS_NO_SHORT_NAMES
#define vec_t       ds_vec_t
#define vec_create  ds_vec_create
#define vec_len     ds_vec_len
#define vec_cap     ds_vec_cap
#define vec_reserve ds_vec_reserve
#define vec_push    ds_vec_push
#define vec_pop     ds_vec_pop
#define vec_rm_swap ds_vec_rm_swap
#define vec_rm      ds_vec_rm
#define vec_free    ds_vec_free
#define vec_get     ds_vec_get
#define vec_set     ds_vec_set
#define vec_qsort   ds_vec_qsort
#define vec_bsearch ds_vec_bsearch
#endif /* DS_NO_SHORT_NAMES */

/* INTERNAL */

// TODO: If you add a function ds_vec_increment_size (probably better to add
// ds_vec_resize and use that) the header definition can become internal
typedef struct {
  uint64_t len : 32;
  uint64_t cap : 32;
} __ds_vec_header_t;

typedef struct {
  __ds_vec_header_t header;
  // Actually a T[], used only for offset calculations
  uint8_t items[];
} __ds_vec_struct_t;

void *__ds_vec_create(size_t val_len, size_t cap);
void *__ds_vec_reserve_internal(void *vec, size_t new_cap, size_t val_len);
size_t __ds_vec_capacity_after_push(void *vec);

/* EXTERNAL */

#define ds_vec_len(VEC) \
  (ds_container_of(VEC, __ds_vec_struct_t, items)->header.len)

#define ds_vec_cap(VEC) \
  (ds_container_of(VEC, __ds_vec_struct_t, items)->header.cap)

#define ds_vec_t(TYPE) TYPE *
// Allocate a new vector with a given initial capacity
#define ds_vec_create(TYPE, CAP) \
  ((ds_vec_t(TYPE)) __ds_vec_create(sizeof(TYPE), CAP))
#define ds_vec_reserve(VEC, NEWCAP) \
  ((VEC) = __ds_vec_reserve_internal(VEC, NEWCAP, sizeof(*VEC)))
#define ds_vec_get(VEC, IDX) \
  (assert(IDX >= 0), assert(IDX < ds_vec_len(VEC)), (VEC)[IDX])
#define ds_vec_set(VEC, IDX, VAL) \
  (assert(IDX >= 0), assert(IDX < ds_vec_len(VEC)), (VEC)[IDX] = (VAL))
#define ds_vec_push(VEC, VAL)                                                \
  (ds_vec_reserve(VEC, ds_vec_cap(VEC) = __ds_vec_capacity_after_push(VEC)), \
   (VEC)[ds_vec_len(VEC)++] = (VAL))
#define ds_vec_qsort(VEC, COMPAR_FN) \
  qsort(VEC, ds_vec_len(VEC), sizeof(*(VEC)), (__compar_fn_t) (COMPAR_FN))
#define ds_vec_bsearch(VEC, KEY, COMPAR_FN) \
  bsearch(KEY, VEC, ds_vec_len(VEC), sizeof(*(VEC)), (__compar_fn_t) COMPAR_FN)

#endif /* DS_VEC_H */
