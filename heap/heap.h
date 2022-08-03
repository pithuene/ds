#ifndef DS_HEAP_H
#define DS_HEAP_H

#include <stdbool.h>
#include <assert.h>

#include "../vec/vec.h"

#ifndef DS_NO_SHORT_NAMES
#define heap_t        ds_heap_t
#define heap_create   ds_heap_create
#define heap_free     ds_heap_free
#define heap_insert   ds_heap_insert
#define heap_peek     ds_heap_peek
#define heap_pop      ds_heap_pop
#define heap_is_empty ds_heap_is_empty
#endif /* DS_NO_SHORT_NAMES */

/* EXTERNAL */
typedef bool(ds_heap_is_lower_than_func_t)(void *, void *);

/* INTERNAL */
typedef struct {
  ds_heap_is_lower_than_func_t *is_lower_than;
  // Actually a vec_t(T)
  void *vector;
} __ds_heap_struct_t;

void *__ds_heap_create(
  size_t val_len, ds_heap_is_lower_than_func_t *is_lower_than
);

void __ds_heap_heapify_up(
  void *heap_vector, size_t val_len, int32_t start_index
);

void __ds_heap_heapify_down(void *heap_vector, size_t val_len);

#define __ds_heap_ensure_tempslot_free(HEAP)                        \
  (ds_vec_len(*(HEAP)) == ds_vec_cap(*(HEAP)))                      \
    ? (ds_vec_reserve(*(HEAP), ds_vec_cap(*(HEAP)) * 2), assert(1)) \
    : assert(1)

/* EXTERNAL */
#define ds_heap_t(TYPE) ds_vec_t(TYPE) *

#define ds_heap_create(TYPE, ISLOWERTHAN) \
  ((ds_heap_t(TYPE)) __ds_heap_create(sizeof(TYPE), ISLOWERTHAN))

void ds_heap_free(void *heap);

bool ds_heap_is_empty(void *heap);

#define ds_heap_insert(HEAP, ...)        \
  (ds_vec_push(*(HEAP), __VA_ARGS__),    \
   __ds_heap_ensure_tempslot_free(HEAP), \
   __ds_heap_heapify_up(HEAP, sizeof(**(HEAP)), ds_vec_len(*(HEAP)) - 1))

#define ds_heap_peek(HEAP) (assert(1), (*(HEAP))[0])

#define ds_heap_pop(HEAP)                             \
  ((*(HEAP))[ds_vec_cap(*(HEAP)) - 1] = (*(HEAP))[0], \
   (*(HEAP))[0] = ds_vec_pop(*(HEAP)),                \
   __ds_heap_heapify_down(HEAP, sizeof(**(HEAP))),    \
   (*(HEAP))[ds_vec_cap(*(HEAP)) - 1])

#endif /* DS_HEAP_H */
