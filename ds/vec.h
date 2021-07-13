#ifndef DS_VEC_H
#define DS_VEC_H

#include <stdlib.h>
#include <string.h>

#define vec_t(TYPE) TYPE *

#define vec ds_vec
#define veclen ds_veclen
#define veccap ds_veccap
#define vecpush ds_vecpush
#define vecpop ds_vecpop
/* Unordered remove */
#define vecrmu ds_vecrmu
/* Ordered remove, use vecrmu if order is irrelevant.
   This removes the element at idx and moves all following elements back. */
#define vecrm ds_vecrm
#define vecfree ds_vecfree

typedef struct {
    size_t len;
    size_t cap;
} DS_VecHeader;

#define ds_vec(TYPE, CAP) \
    ((vec_t(TYPE)) ds_vec_new(sizeof(TYPE), CAP))

void * ds_vec_new(size_t val_len, size_t cap);
DS_VecHeader * ds_vec_header(void * v);
void ds_vecfree(void * vec);

#define ds_vec_ensure_capacity(VEC, COUNT) ( \
    ds_vec_grow(VEC, ds_vec_grow_capacity(VEC, COUNT)) \
)

/* Calculates the new number of elements after possibly growing to ensure
 * sufficient capacity for new_elem_count new entries */
size_t ds_vec_grow_capacity(void * vec, size_t new_elem_count);

#define ds_vec_grow(VEC, NEWCAP) ((VEC) = ds_vec_grow_internal(VEC, NEWCAP, sizeof(*VEC)))

void * ds_vec_grow_internal(void * vec, size_t new_cap, size_t val_len);

#define ds_vecpush(VEC, ELEM) ( \
    ds_vec_ensure_capacity(VEC, 1), \
    (VEC)[ds_vec_header(VEC)->len++] = (ELEM))

size_t ds_veclen(void * vec);

size_t ds_veccap(void * vec);

#define ds_vecpop(VEC) (VEC[--ds_vec_header(VEC)->len])

#define ds_vecrmu(VEC, IDX) (VEC[IDX]); (VEC[IDX] = ds_vecpop(VEC))

#define ds_vecrm(VEC, IDX) (VEC[IDX]); (ds_vecrm_internal(VEC, sizeof(*VEC), IDX))

void ds_vecrm_internal(void * v, size_t stride, size_t idx);

#endif
