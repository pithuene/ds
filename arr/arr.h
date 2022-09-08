#ifndef DS_ARR_H
#define DS_ARR_H

#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <assert.h>
#include "../util/container_of.h"

#ifndef DS_NO_SHORT_NAMES
#define arr_t       ds_arr_t
#define arr_create  ds_arr_create
#define arr_len     ds_arr_len
#define arr_get     ds_arr_get
#define arr_set     ds_arr_set
#define arr_qsort   ds_arr_qsort
#define arr_bsearch ds_arr_bsearch
#endif /* DS_NO_SHORT_NAMES */

struct __ds_arr_struct_t {
  uint64_t len;
  uint64_t values[];
};

#define ds_arr_t(TYPE) TYPE *

#define ds_arr_create(TYPE, LENGTH, ...) \
  ((TYPE *) &(struct {                   \
     uint64_t len;                       \
     TYPE values[LENGTH];                \
   }){LENGTH, __VA_ARGS__}               \
     .values)

#define ds_arr_len(ARR) \
  (ds_container_of(ARR, struct __ds_arr_struct_t, values)->len)

#define ds_arr_get(ARR, IDX) \
  (assert(IDX >= 0), assert(IDX < ds_arr_len(ARR)), ARR[IDX])

#define ds_arr_set(ARR, IDX, VAL) \
  (assert(IDX >= 0), assert(IDX < ds_arr_len(ARR)), ARR[IDX] = (VAL))

#define ds_arr_qsort(ARR, COMPAR_FN) \
  qsort(ARR, ds_arr_len(ARR), sizeof(*(ARR)), (__compar_fn_t) (COMPAR_FN))

#define ds_arr_bsearch(ARR, KEY, COMPAR_FN) \
  bsearch(KEY, ARR, ds_arr_len(ARR), sizeof(*(ARR)), (__compar_fn_t) COMPAR_FN)

#endif /* DS_ARR_H */
