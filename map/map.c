#include "map.h"
#include <assert.h>
#include <stdlib.h>
#include <string.h>

#include "../util/container_of.h"

/* NOT EXPOSED */

// Check if v is a power of two
// https://graphics.stanford.edu/~seander/bithacks.html#DetermineIfPowerOf2
static inline bool is_pow2(uint32_t v) { return v && !(v & (v - 1)); }

// Calculate x mod y where y = 2^p for some p
// https://graphics.stanford.edu/~seander/bithacks.html#ModulusDivisionEasy
static inline uint32_t mod_pow2(uint32_t x, uint32_t y) {
  assert(is_pow2(y));
  return (x & (y - 1));
}

// Calculate the byte size of the filled / tombstone bitmap
// The size is rounded up to a multiple of 8 bytes to avoid alignment issues.
static inline size_t calculate_ft_bitmap_size(size_t bucket_count) {
  size_t bits = 2 * bucket_count;  // 2 bits per bucket
  if (bits % 64 != 0) {
    // Round up to 8 byte multiple
    // 8byte = 64bit
    bits = bits + 64 - (bits % 64);
    assert(bits % 64 == 0);
  }
  return bits / 8;
}

static inline bool ft_is_full(uint8_t *ft_bitmap, uint32_t bucket_index) {
  const uint32_t byte_index = bucket_index / 4;
  const uint32_t bit_offset = 7 - ((bucket_index % 4) * 2);
  uint8_t byte = ft_bitmap[byte_index];
  bool is_full = (byte >> bit_offset) & 0x01;
  return is_full;
}

static inline bool ft_has_tombstone(uint8_t *ft_bitmap, uint32_t bucket_index) {
  const uint32_t byte_index = bucket_index / 4;
  const uint32_t bit_offset = 6 - ((bucket_index % 4) * 2);
  uint8_t byte = ft_bitmap[byte_index];
  bool is_full = (byte >> bit_offset) & 0x01;
  return is_full;
}

static inline void ft_set_full(
  uint8_t *ft_bitmap, uint32_t bucket_index, bool full
) {
  const uint32_t byte_index = bucket_index / 4;
  const uint32_t bit_offset = (bucket_index % 4) * 2;
  uint8_t mask = (0x80 >> bit_offset);
  if (full) {
    ft_bitmap[byte_index] |= mask;
  } else {
    ft_bitmap[byte_index] &= ~mask;
  }
}

static inline void ft_set_tombstone(
  uint8_t *ft_bitmap, uint32_t bucket_index, bool tombstone
) {
  const uint32_t byte_index = bucket_index / 4;
  const uint32_t bit_offset = (bucket_index % 4) * 2 + 1;
  uint8_t mask = (0x80 >> bit_offset);
  if (tombstone) {
    ft_bitmap[byte_index] |= mask;
  } else {
    ft_bitmap[byte_index] &= ~mask;
  }
}

// Calculate the byte length of the keys section.
// Size is rounded up to a multiple of 8 bytes.
static inline size_t calculate_keys_mem_len(
  size_t key_len, size_t bucket_count
) {
  size_t mem_len = key_len * bucket_count;
  if (mem_len % 8 != 0) {
    // Round up to 8 byte multiple
    mem_len = mem_len + 8 - (mem_len % 8);
  }
  assert(mem_len % 8 == 0);
  return mem_len;
}

// Calculates the size of the memory block of a map of bucket_count entries
static inline size_t calculate_mem_len(
  size_t bucket_count, size_t key_len, size_t val_len
) {
  return calculate_keys_mem_len(key_len, bucket_count)
       + calculate_ft_bitmap_size(bucket_count) + sizeof(__ds_map_struct_t)
       + val_len * bucket_count;
}

// Compute the returned pointer from the beginning of the memory block
static inline void *map_from_keys(
  void *keys, size_t key_len, size_t bucket_count
) {
  const size_t offset = calculate_keys_mem_len(key_len, bucket_count)
                      + calculate_ft_bitmap_size(bucket_count)
                      + sizeof(__ds_map_struct_t);
  return ((char *) keys) + offset;
}

// Compute the map header from a pointer to the maps first value.
static inline uint8_t *ft_bitmap_from_header(__ds_map_struct_t *header) {
  return (uint8_t
            *) (((char *) header) - calculate_ft_bitmap_size(header->cap));
}

static inline void *keys_from_header(__ds_map_struct_t *header) {
  const size_t offset = calculate_keys_mem_len(header->key_len, header->cap)
                      + calculate_ft_bitmap_size(header->cap);
  return ((char *) header) - offset;
}

static inline void *get_key(void *keys, size_t key_len, uint32_t bucket_index) {
  return ((char *) keys) + (key_len * bucket_index);
}

// Calculate the memory position of a given bucket
static inline void *get_value(
  void *map, size_t val_len, uint32_t bucket_index
) {
  return ((char *) map) + (val_len * bucket_index);
}

// https://graphics.stanford.edu/~seander/bithacks.html#RoundUpPowerOf2
static inline uint32_t calculate_next_pow_2(uint32_t v) {
  v--;
  v |= v >> 1;
  v |= v >> 2;
  v |= v >> 4;
  v |= v >> 8;
  v |= v >> 16;
  v++;
  return v;
}

// Calculate the appropriate number of buckets for a given number of entries.
static inline uint32_t capacity_for_entry_count(uint32_t entry_count) {
  // Make sure the map is only 75% full when all entries are inserted
  const uint32_t desired_bucket_count = entry_count * 4 / 3;
  const uint32_t next_pow_2 = calculate_next_pow_2(desired_bucket_count);
  return next_pow_2;
}

static uint64_t ds_map_hash_default(void *key, size_t key_len) {
  uint8_t *input = key;
  uint64_t hash = 5381;
  for (size_t i = 0; i < key_len; i++) {
    uint64_t c = *input++;
    hash = ((hash << 5) + hash) + c; /* hash * 33 + c */
  }
  return hash;
}

bool ds_map_equals_default(void *key_a, void *key_b, size_t key_len) {
  return memcmp(key_a, key_b, key_len) == 0;
}

/* INTERNAL */

void *__ds_map_create_internal(
  size_t key_len,
  size_t val_len,
  size_t entry_count,
  ds_map_hash_func_t *hash_func,
  ds_map_equals_func_t *equals_func
) {
  // Leave enough room, so the map is only 75% filled when the desired items are
  // inserted.
  const size_t bucket_count = capacity_for_entry_count(entry_count);
  const size_t mem_len = calculate_mem_len(bucket_count, key_len, val_len);

  // calloc zeroes out the bitmap
  void *keys = calloc(1, mem_len);
  void *map = map_from_keys(keys, key_len, bucket_count);

  __ds_map_struct_t *header = ds_container_of(map, __ds_map_struct_t, buckets);
  *header = (__ds_map_struct_t){
    .size = 0,
    .cap = bucket_count,
    .tombstone_count = 0,
    .key_len = key_len,
    .hash_func = hash_func ? hash_func : ds_map_hash_default,
    .equals_func = equals_func ? equals_func : ds_map_equals_default,
  };

  return map;
}

/* Finds an appropriate bucket for a new entry with a given key.
 * If the key is already present, that keys bucket is returned.
 * Otherwise an empty one is.
 * Also sets the bitmap entries and copies stores the key.
 * Returns the index of the bucket.*/
uint32_t __ds_map_alloc_bucket(void *map, void *key, size_t val_len) {
  __ds_map_struct_t *header = ds_container_of(map, __ds_map_struct_t, buckets);
  void *keys = keys_from_header(header);
  uint8_t *ft_bitmap = ft_bitmap_from_header(header);

  int64_t tombstone_index = -1;
  uint32_t bucket_index =
    mod_pow2((*header->hash_func)(key, header->key_len), header->cap);
  while (ft_has_tombstone(ft_bitmap, bucket_index) ||
         (ft_is_full(ft_bitmap, bucket_index) &&
          !(*header->equals_func)(get_key(keys, header->key_len, bucket_index),
                                  key, header->key_len))) {
    if (ft_has_tombstone(ft_bitmap, bucket_index) && tombstone_index < 0) {
      tombstone_index = bucket_index;
    }
    // Linear probing
    bucket_index++;
    bucket_index = mod_pow2(bucket_index, header->cap);
  }

  if (!ft_is_full(ft_bitmap, bucket_index)) {
    // Key is not there
    if (tombstone_index >= 0) {
      // Skipped over a tombstone. Reuse its bucket.
      bucket_index = tombstone_index;
      header->tombstone_count--;
      ft_set_tombstone(ft_bitmap, bucket_index, false);
    }
    header->size++;
    ft_set_full(ft_bitmap, bucket_index, true);
  }

  // Save the allocated key
  void *stored_key = get_key(keys, header->key_len, bucket_index);
  memcpy(stored_key, key, header->key_len);

  return bucket_index;
}

void *__ds_map_reserve_internal(
  void *old_map, size_t new_entry_count, size_t val_len
) {
  __ds_map_struct_t *old_header =
    ds_container_of(old_map, __ds_map_struct_t, buckets);
  const size_t new_cap = capacity_for_entry_count(new_entry_count);
  if (old_header->cap >= new_cap) return old_map;
  void *new_keys =
    calloc(1, calculate_mem_len(new_cap, old_header->key_len, val_len));
  void *new_map = map_from_keys(new_keys, old_header->key_len, new_cap);
  __ds_map_struct_t *new_header =
    ds_container_of(new_map, __ds_map_struct_t, buckets);
  *new_header = (__ds_map_struct_t){
    .cap = new_cap,
    .size = 0,
    .tombstone_count = 0,
    .key_len = old_header->key_len,
    .equals_func = old_header->equals_func,
    .hash_func = old_header->hash_func,
  };

  uint8_t *old_ft_bitmap = ft_bitmap_from_header(old_header);

  void *old_keys = keys_from_header(old_header);

  /* Rehash all entries and copy their values over */
  for (uint32_t old_idx = 0; old_idx < old_header->cap; old_idx++) {
    if (ft_is_full(old_ft_bitmap, old_idx)) {
      void *old_key = get_key(old_keys, old_header->key_len, old_idx);
      uint32_t new_idx = __ds_map_alloc_bucket(new_map, old_key, val_len);
      // Copy the value over into the new map
      void *old_value = get_value(old_map, val_len, old_idx);
      void *new_value = get_value(new_map, val_len, new_idx);
      memcpy(new_value, old_value, val_len);
    }
  }

  free(old_keys);

  return new_map;
}

uint32_t __ds_map_get_internal(void *map, void *key, size_t val_len) {
  __ds_map_struct_t *header = ds_container_of(map, __ds_map_struct_t, buckets);
  void *keys = keys_from_header(header);
  uint8_t *ft_bitmap = ft_bitmap_from_header(header);

  uint32_t bucket_index =
    mod_pow2((*header->hash_func)(key, header->key_len), header->cap);
  while (ft_has_tombstone(ft_bitmap, bucket_index) ||
         (ft_is_full(ft_bitmap, bucket_index) &&
          !(*header->equals_func)(get_key(keys, header->key_len, bucket_index),
                                  key, header->key_len))) {
    // Linear probing
    bucket_index++;
    bucket_index = mod_pow2(bucket_index, header->cap);
  }

  // At this point, either a full bucket is reached, in which case it contains
  // the correct key or an empty bucket is reached, in which case the key is not
  // inside the map.

  if (ft_is_full(ft_bitmap, bucket_index)) {
    // Hit
    return bucket_index;
  } else {
    // Key not found
    return __ds_map_null_index;
  }
}

bool __ds_map_remove_internal(void *map, void *key, size_t val_len) {
  uint32_t bucket_index = __ds_map_get_internal(map, key, val_len);
  if (bucket_index == __ds_map_null_index) return false;

  __ds_map_struct_t *header = ds_container_of(map, __ds_map_struct_t, buckets);
  uint8_t *ft_bitmap = ft_bitmap_from_header(header);
  ft_set_tombstone(ft_bitmap, bucket_index, true);
  header->tombstone_count++;
  return true;
}

// Get a pointer to the key of some bucket
void *__ds_map_get_key(void *map, uint32_t bucket_index) {
  __ds_map_struct_t *header = ds_container_of(map, __ds_map_struct_t, buckets);
  void *keys = keys_from_header(header);
  return get_key(keys, header->key_len, bucket_index);
}

bool __ds_map_is_index_filled(void *map, uint32_t index) {
  __ds_map_struct_t *header = ds_container_of(map, __ds_map_struct_t, buckets);
  uint8_t *ft_bitmap = ft_bitmap_from_header(header);
  return ft_is_full(ft_bitmap, index);
}

/* EXTERNAL */

const uint32_t __ds_map_null_index = UINT32_MAX;

uint32_t ds_map_cap(void *map) {
  return ds_container_of(map, __ds_map_struct_t, buckets)->cap;
}

uint32_t ds_map_size(void *map) {
  return ds_container_of(map, __ds_map_struct_t, buckets)->size;
}

void ds_map_free(void *map) {
  __ds_map_struct_t *header = ds_container_of(map, __ds_map_struct_t, buckets);
  void *keys = keys_from_header(header);
  free(keys);
}
