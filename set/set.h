#ifndef DS_SET_H
#define DS_SET_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <limits.h>

#ifndef DS_NO_SHORT_NAMES
  #define set_t       ds_set_t
  #define set_create  ds_set_create
  #define set_reserve ds_set_reserve
  #define set_cap     ds_set_cap
  #define set_size    ds_set_size
  #define set_add     ds_set_add
  #define set_has     ds_set_has
  #define set_get     ds_set_get
  #define set_remove  ds_set_remove
  #define set_free    ds_set_free
#endif /* DS_NO_SHORT_NAMES */

typedef uint64_t (ds_set_hash_func_t)(void *key, size_t key_len);
typedef bool (ds_set_equals_func_t)(void *key_a, void *key_b, size_t key_len);

/* INTERNAL */

typedef struct {
  /* The numer of filled buckets */
  uint32_t size;
  /* The numer of buckets */
  uint32_t cap;
  /* The number of tombstones in the set */
  uint32_t tombstone_count;
  /* Function with which set keys are hashed */
  ds_set_hash_func_t *hash_func;
  /* Function with which set keys are compared */
  ds_set_equals_func_t *equals_func;
} __ds_set_header_t;

void *__ds_set_create_internal(
  size_t val_len,
  size_t cap,
  ds_set_hash_func_t *hash_func,
  ds_set_equals_func_t *equals_func
);

void *__ds_set_reserve_internal(void *old_set, size_t new_cap, size_t val_len);
uint32_t __ds_set_alloc_bucket(void *set, void *key, size_t val_len);
uint32_t __ds_set_get_internal(void *set, void *key, size_t val_len);
bool __ds_set_remove_internal(void *set, void *key, size_t val_len);

const uint32_t __ds_set_null_index = UINT32_MAX;

/* EXTERNAL */

uint32_t ds_set_cap(void *set);
uint32_t ds_set_size(void *set);
void ds_set_free(void *set);

#define ds_set_t(TYPE) TYPE *
#define ds_set_create(TYPE, CAP, HASH_FUNC, EQUALS_FUNC) \
  (__ds_set_create_internal(sizeof(TYPE), CAP, HASH_FUNC, EQUALS_FUNC))
#define ds_set_reserve(SET, NEWCAP) \
  ((SET) = __ds_set_reserve_internal(SET, NEWCAP, sizeof(*SET)))
#define ds_set_add(SET, ...) ( \
  (SET)[0] = (__VA_ARGS__), \
  ds_set_reserve(SET, ds_set_size(SET) + 1), \
  (SET)[__ds_set_alloc_bucket(SET, &(SET)[0], sizeof(*(SET))) + 1] = (SET)[0] \
)
#define ds_set_has(SET, ...) ( \
  (SET)[0] = (__VA_ARGS__), \
  __ds_set_get_internal(SET, &(SET)[0], sizeof(*SET)) != __ds_set_null_index \
)
#define ds_set_get(SET, ...) ( \
  (SET)[0] = (__VA_ARGS__), \
  (SET)[__ds_set_get_internal(SET, &(SET)[0], sizeof(*SET)) + 1] \
)
#define ds_set_remove(SET, ...) ( \
  (SET)[0] = (__VA_ARGS__), \
  __ds_set_remove_internal(SET, &(SET)[0], sizeof(*SET)) \
)

#endif /* DS_SET_H */
