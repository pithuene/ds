#include <stdlib.h>
#include <string.h>

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
    ((TYPE *) ds_vec_new(sizeof(TYPE), CAP))

void * ds_vec_new(size_t val_len, size_t cap) {
    DS_VecHeader * ptr = (DS_VecHeader *) malloc(sizeof(DS_VecHeader) + cap * val_len);
    ptr->len = 0;
    ptr->cap = cap;
    return (char *) ptr + sizeof(DS_VecHeader);
}

DS_VecHeader * ds_vec_header(void * v) {
    return (DS_VecHeader *) ((char *) v - sizeof(DS_VecHeader));
}

void ds_vecfree(void * vec) {
  free(ds_vec_header(vec));
}

#define ds_vec_ensure_capacity(VEC, COUNT) ( \
    ds_vec_grow(VEC, ds_vec_grow_capacity(VEC, COUNT)) \
)

/* Calculates the new number of elements after possibly growing to ensure
 * sufficient capacity for new_elem_count new entries */
size_t ds_vec_grow_capacity(void * vec, size_t new_elem_count) {
  DS_VecHeader * header = ds_vec_header(vec);
  size_t new_cap = header->cap;
  while (new_cap < header->len + new_elem_count) {
    new_cap *= 2;
  }
  return new_cap;
}

#define ds_vec_grow(VEC, NEWCAP) ((VEC) = ds_vec_grow_internal(VEC, NEWCAP, sizeof(*VEC)))

void * ds_vec_grow_internal(void * vec, size_t new_cap, size_t val_len) {
  DS_VecHeader * header = ds_vec_header(vec);
  if (header->cap == new_cap) return vec;
  header = (DS_VecHeader *) realloc(header, sizeof(DS_VecHeader) + new_cap * val_len);
  header->cap = new_cap;
  return (char *) header + sizeof(DS_VecHeader);
}

#define ds_vecpush(VEC, ELEM) ( \
    ds_vec_ensure_capacity(VEC, 1), \
    (VEC)[ds_vec_header(VEC)->len++] = (ELEM))

size_t ds_veclen(void * vec) {
  return ds_vec_header(vec)->len;
}

size_t ds_veccap(void * vec) {
  return ds_vec_header(vec)->cap;
}

#define ds_vecpop(VEC) (VEC[--ds_vec_header(VEC)->len])

#define ds_vecrmu(VEC, IDX) (VEC[IDX]); (VEC[IDX] = ds_vecpop(VEC))


#define ds_vecrm(VEC, IDX) (VEC[IDX]); (ds_vecrm_internal(VEC, sizeof(*VEC), IDX))

void ds_vecrm_internal(void * v, size_t stride, size_t idx) {
    DS_VecHeader * header = ds_vec_header(v);
    void * dest = (char *) v + stride * idx;
    void * src = (char *) v + stride * (idx + 1);
    memmove(dest, src, (header->len - idx - 1) * stride);
    header->len--;
}
