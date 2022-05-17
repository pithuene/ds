#ifndef DS_ARR_H
#define DS_ARR_H

#include <stddef.h>
#include <assert.h>

#ifndef DS_NO_SHORT_NAMES
  #define arr_t      ds_arr_t
  #define arr_create ds_arr_create
  #define arr_len    ds_arr_len
  #define arr_get    ds_arr_get
  #define arr_set    ds_arr_set
#endif /* DS_NO_SHORT_NAMES */

#define ds_arr_t(TYPE) TYPE *

#define ds_arr_create(TYPE, LENGTH, ...) \
  ((TYPE *) (((char *) &(struct {size_t length; TYPE values[LENGTH]; }){LENGTH, __VA_ARGS__}) + sizeof(size_t)))

#define ds_arr_len(ARR) (*((size_t *) (((char *) ARR) - sizeof(size_t))))

#define ds_arr_get(ARR, IDX) (assert(IDX >= 0), assert(IDX < ds_arr_len(ARR)), ARR[IDX])

#define ds_arr_set(ARR, IDX, VAL) (assert(IDX >= 0), assert(IDX < ds_arr_len(ARR)), ARR[IDX] = (VAL))

#endif /* DS_ARR_H */
