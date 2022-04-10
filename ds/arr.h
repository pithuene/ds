#ifndef DS_ARR_H
#define DS_ARR_H

#include <stddef.h>

#define arr_t      ds_arr_t
#define arr_create ds_arr_create
#define arr_len    ds_arr_len

#define ds_arr_t(TYPE) TYPE *

#define ds_arr_create(TYPE, LENGTH, ...) \
  ((TYPE *) (((char *) &(struct {size_t length; TYPE values[LENGTH]; }){LENGTH, __VA_ARGS__}) + sizeof(size_t)))

#define ds_arr_len(ARR) (*((size_t *) (((char *) ARR) - sizeof(size_t))))

#endif
