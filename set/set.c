#include "set.h"
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

/* NOT EXPOSED */

// Calculate the byte size of the filled / tombstone bitmap
// The size is rounded up to a multiple of 8 bytes to avoid alignment issues.
static inline size_t calculate_ft_bitmap_size(size_t bucket_count) {
  size_t bits = 2 * bucket_count; // 2 bits per bucket
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

static inline void ft_set_full(uint8_t *ft_bitmap, uint32_t bucket_index, bool full) {
  const uint32_t byte_index = bucket_index / 4;
  const uint32_t bit_offset = (bucket_index % 4) * 2;
  uint8_t mask = (0x80 >> bit_offset);
  if (full) {
    ft_bitmap[byte_index] |= mask;
  } else {
    ft_bitmap[byte_index] &= ~mask;
  }
}

static inline void ft_set_tombstone(uint8_t *ft_bitmap, uint32_t bucket_index, bool tombstone) {
  const uint32_t byte_index = bucket_index / 4;
  const uint32_t bit_offset = (bucket_index % 4) * 2 + 1;
  uint8_t mask = (0x80 >> bit_offset);
  if (tombstone) {
    ft_bitmap[byte_index] |= mask;
  } else {
    ft_bitmap[byte_index] &= ~mask;
  }
}

// Calculates the size of the memory block of a set of bucket_count entries
static inline size_t calculate_mem_len(size_t bucket_count, size_t val_len) {
  return calculate_ft_bitmap_size(bucket_count)
       + sizeof(__ds_set_header_t)
       + (val_len + 1) * bucket_count;
}

// Compute the set header from a pointer to the sets first value.
static inline __ds_set_header_t *header_from_set(void *set) {
  return (__ds_set_header_t *) (((char *) set) - sizeof(__ds_set_header_t));
}

// Add the size of the ft_bitmap and the set header to the pointer
static inline void *set_from_ft_bitmap(uint8_t *ft_bitmap, size_t bucket_count) {
  return (void *) (((char *) ft_bitmap)
    + calculate_ft_bitmap_size(bucket_count)
    + sizeof(__ds_set_header_t));
}

static inline uint8_t *ft_bitmap_from_header(__ds_set_header_t *header) {
  return (uint8_t *) (((char *) header) - calculate_ft_bitmap_size(header->cap));
}

// Calculate the memory position of a given bucket
static inline void *get_value(void *set, size_t val_len, uint32_t bucket_index) {
  return ((char *) set) + (val_len * (bucket_index + 1));
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
  // Make sure the set is only 75% full when all entries are inserted
  const uint32_t desired_bucket_count = entry_count * 4 / 3;
  const uint32_t next_pow_2 = calculate_next_pow_2(desired_bucket_count);
  return next_pow_2;
}

static uint64_t set_hash_default(void *value, size_t val_len) {
  uint8_t *input = value;
  uint64_t hash = 5381;
  for (size_t i = 0; i < val_len; i++) {
    uint64_t c = *input++;
    hash = ((hash << 5) + hash) + c; /* hash * 33 + c */
  }
  return hash;
}

bool set_equals_default(void *value_a, void *value_b, size_t val_len) {
  return memcmp(value_a, value_b, val_len) == 0;
}

/* INTERNAL */

void *__ds_set_create_internal(
  size_t val_len,
  size_t entry_count,
  ds_set_hash_func_t *hash_func,
  ds_set_equals_func_t *equals_func
) {
  // Leave enough room, so the set is only 75% filled when the desired items are inserted.
  const size_t bucket_count = capacity_for_entry_count(entry_count);
  const size_t mem_len = calculate_mem_len(bucket_count, val_len);

  // calloc zeroes out the bitmap
  uint8_t *ft_bitmap = calloc(1, mem_len);
  void *set = set_from_ft_bitmap(ft_bitmap, bucket_count);

  __ds_set_header_t *header = header_from_set(set);
  *header = (__ds_set_header_t){
    .size = 0,
    .cap = bucket_count,
    .tombstone_count = 0,
    .hash_func = (hash_func) ? hash_func : set_hash_default,
    .equals_func = (equals_func) ? equals_func : set_equals_default,
  };

  return set;
}

/* Finds an appropriate bucket for a new entry with a given key.
 * If the key is already present, that keys bucket is returned.
 * Otherwise an empty one is.
 * Also sets the bitmap entries and copies stores the key.
 * Returns the index of the bucket.*/
uint32_t __ds_set_alloc_bucket(void *set, void *key, size_t val_len) {
  __ds_set_header_t *header = header_from_set(set);
  uint8_t *ft_bitmap = ft_bitmap_from_header(header);

  uint32_t bucket_index = (*header->hash_func)(key, val_len) % header->cap;
  while (ft_is_full(ft_bitmap, bucket_index)
    && !(*header->equals_func)(get_value(set, val_len, bucket_index), key, val_len)
  ) {
    // Linear probing
    bucket_index++;
    bucket_index %= header->cap;
  }

  // Adjust map metadata
  if (!ft_is_full(ft_bitmap, bucket_index)) {
    header->size++;
  }
  if (ft_has_tombstone(ft_bitmap, bucket_index)) {
    header->tombstone_count--;
  }

  // Update filled and tombstone in bitmap
  ft_set_full(ft_bitmap, bucket_index, true);
  ft_set_tombstone(ft_bitmap, bucket_index, false);

  return bucket_index;
}

void *__ds_set_reserve_internal(void *old_set, size_t new_entry_count, size_t val_len) {
  __ds_set_header_t *old_header = header_from_set(old_set);
  const size_t new_cap = capacity_for_entry_count(new_entry_count);
  if (old_header->cap >= new_cap) return old_set;
  uint8_t *new_ft_bitmap = calloc(1, calculate_mem_len(new_cap, val_len));
  void *new_set = set_from_ft_bitmap(new_ft_bitmap, new_cap);
  __ds_set_header_t *new_header = header_from_set(new_set);
  *new_header = (__ds_set_header_t){
    .cap = new_cap,
    .size = 0,
    .tombstone_count = 0,
    .equals_func = old_header->equals_func,
    .hash_func = old_header->hash_func,
  };

  uint8_t *old_ft_bitmap = ft_bitmap_from_header(old_header);

  /* Rehash all entries and copy their values over */
  for (uint32_t old_idx = 0; old_idx < old_header->cap; old_idx++) {
    if (ft_is_full(old_ft_bitmap, old_idx)) {
      void *old_value = get_value(old_set, val_len, old_idx);
      uint32_t new_idx = __ds_set_alloc_bucket(new_set, old_value, val_len);
      // Copy the value over into the new set
      void *new_value = get_value(new_set, val_len, new_idx);
      memcpy(new_value, old_value, val_len);
    }
  }

  free(old_ft_bitmap);
  
  return new_set;
}

uint32_t __ds_set_get_internal(void *set, void *key, size_t val_len) {
  __ds_set_header_t *header = header_from_set(set);
  uint8_t *ft_bitmap = ft_bitmap_from_header(header);

  uint32_t bucket_index = (*header->hash_func)(key, val_len) % header->cap;
  while (ft_has_tombstone(ft_bitmap, bucket_index)
    || (ft_is_full(ft_bitmap, bucket_index) && !(*header->equals_func)(get_value(set, val_len, bucket_index), key, val_len)
  )) {
    // Linear probing
    bucket_index++;
    bucket_index %= header->cap;
  }

  // At this point, either a full bucket is reached, in which case it contains the correct key
  // or an empty bucket is reached, in which case the key is not inside the set.

  if (ft_is_full(ft_bitmap, bucket_index)) {
    // Hit
    return bucket_index;
  } else {
    // Key not found
    return __ds_set_null_index;
  }
}

bool __ds_set_remove_internal(void *set, void *key, size_t val_len) {
  uint32_t bucket_index = __ds_set_get_internal(set, key, val_len);
  if (bucket_index == __ds_set_null_index) return false;
  
  __ds_set_header_t *header = header_from_set(set);
  uint8_t *ft_bitmap = ft_bitmap_from_header(header);
  ft_set_tombstone(ft_bitmap, bucket_index, true);
  return true;
}

/* EXTERNAL */

uint32_t ds_set_cap(void *set) {
  return header_from_set(set)->cap;
}

uint32_t ds_set_size(void *set) {
  return header_from_set(set)->size;
}

void ds_set_free(void *set) {
  __ds_set_header_t *header = header_from_set(set);
  free(ft_bitmap_from_header(header));
}

