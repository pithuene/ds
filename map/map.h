#ifndef DS_MAP_H
#define DS_MAP_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <limits.h>

#ifndef DS_NO_SHORT_NAMES
  #define map_t       ds_map_t
  #define map_create  ds_map_create
  #define map_reserve ds_map_reserve
  #define map_cap     ds_map_cap
  #define map_size    ds_map_size
  #define map_put     ds_map_put
  #define map_has     ds_map_has
  #define map_get     ds_map_get
  #define map_remove  ds_map_remove
  #define map_free    ds_map_free
#endif /* DS_NO_SHORT_NAMES */

typedef uint64_t (ds_map_hash_func_t)(void *key, size_t key_len);
typedef bool (ds_map_equals_func_t)(void *key_a, void *key_b, size_t key_len);

/* INTERNAL */

typedef struct {
  /* The numer of filled buckets */
  uint32_t size;
  /* The numer of buckets */
  uint32_t cap;
  /* The number of tombstones in the map */
  uint32_t tombstone_count;
  /* The byte length of the key datatype */
  uint32_t key_len;
  /* Function with which map keys are hashed */
  ds_map_hash_func_t *hash_func;
  /* Function with which map keys are compared */
  ds_map_equals_func_t *equals_func;
} __ds_map_header_t;

void *__ds_map_create_internal(
  size_t key_len,
  size_t val_len,
  size_t cap,
  ds_map_hash_func_t *hash_func,
  ds_map_equals_func_t *equals_func
);

void *__ds_map_reserve_internal(void *old_map, size_t new_cap, size_t val_len);
uint32_t __ds_map_alloc_bucket(void *map, void *key, size_t val_len);
uint32_t __ds_map_get_internal(void *map, void *key, size_t val_len);
bool __ds_map_remove_internal(void *map, void *key, size_t val_len);

const uint32_t __ds_map_null_index = UINT32_MAX;

/* EXTERNAL */

uint32_t ds_map_cap(void *map);
uint32_t ds_map_size(void *map);
void ds_map_free(void *map);

// The KEY_TYPE is not actually used,
// but listed for readability of the calling code.
#define ds_map_t(KEY_TYPE, VALUE_TYPE) VALUE_TYPE *
#define ds_map_create(KEY_TYPE, VALUE_TYPE, CAP, HASH_FUNC, EQUALS_FUNC) \
  (__ds_map_create_internal(sizeof(KEY_TYPE), sizeof(VALUE_TYPE), CAP, HASH_FUNC, EQUALS_FUNC))
#define ds_map_reserve(MAP, NEWCAP) \
  ((MAP) = __ds_map_reserve_internal(MAP, NEWCAP, sizeof(*MAP)))
#define ds_map_put(MAP, KEY, VALUE) ( \
  ds_map_reserve(MAP, ds_map_size(MAP) + 1), \
  (MAP)[__ds_map_alloc_bucket(MAP, KEY, sizeof(*(MAP)))] = (VALUE) \
)
#define ds_map_has(MAP, KEY) (__ds_map_get_internal(MAP, KEY, sizeof(*MAP)) != __ds_map_null_index)
#define ds_map_get(MAP, KEY) ((MAP)[__ds_map_get_internal(MAP, KEY, sizeof(*MAP))])
#define ds_map_remove(MAP, KEY) (__ds_map_remove_internal(MAP, KEY, sizeof(*MAP)))

#endif /* DS_MAP_H */
