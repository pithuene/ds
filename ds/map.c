#include "map.h"

/* Allocates a new empty map for cap values of size val_len */
void * ds_map_new(size_t val_len, size_t cap) {
  size_t mem_len = sizeof(DS_MapHeader) + cap * val_len;
  DS_MapHeader * ptr = (DS_MapHeader *) malloc(mem_len);
  memset(ptr, 0, mem_len);
  ptr->size = 0;
  ptr->cap = cap;
  ptr->tombstone_count = 0;
  return (char *) ptr + sizeof(DS_MapHeader);
}

/* Returns the pointer to the map header stored in front */
DS_MapHeader * ds_map_header(void * v) {
    return (DS_MapHeader *) ((char *) v - sizeof(DS_MapHeader));
}

/* Calculates the capacity a map should grow to before adding new_elem_count elements */
size_t ds_map_grow_capacity(void * map, size_t new_elem_count) {
  DS_MapHeader * header = ds_map_header(map);
  size_t new_cap = header->cap;
  float required_capacity = (float) (header->size + new_elem_count);
  while (new_cap / 4 * 3 < required_capacity) {
    new_cap *= 2;
  }
  return new_cap;
}

/* Calculates the byte length of a bucket for a given val_len */
size_t ds_map_bucket_len(size_t val_len) {
  return sizeof(DS_MapBucketHeader) + val_len;
}

/* Returns a pointer to idx-th bucket header */
DS_MapBucketHeader * ds_map_bucket_header(void * map, int idx, size_t val_len) {
  DS_MapHeader * header = ds_map_header(map);
  size_t bucket_len = ds_map_bucket_len(val_len);
  return (DS_MapBucketHeader *) ((char *) map + idx * bucket_len);
}

/* Calculates the size of the memory block of a map of cap entries with length val_len */
size_t ds_map_mem_len(size_t cap, size_t val_len) {
  return sizeof(DS_MapHeader) + cap * ds_map_bucket_len(val_len);
}

/* djb2 string hash function */
unsigned long ds_strhash(unsigned char *str) {
  unsigned long hash = 5381;
  int c;
  while (c = *str++)
    hash = ((hash << 5) + hash) + c; /* hash * 33 + c */
  return hash;
}

/* Retrieves the index of a given key in a maps bucket array */
int ds_map_get_key_idx(void * map, char * key, size_t val_len) {
  DS_MapHeader * mapHeader = ds_map_header(map);
  unsigned long bucketIndex = ds_strhash((unsigned char *) key) % mapHeader->cap;
  DS_MapBucketHeader * bucketHeader = ds_map_bucket_header(map, bucketIndex, val_len);
  while (bucketHeader->tombstone || (bucketHeader->full && strcmp(bucketHeader->key, key))) {
    bucketIndex++;
    bucketIndex %= mapHeader->cap;
    bucketHeader = ds_map_bucket_header(map, bucketIndex, val_len);
  }
  if (bucketHeader->full) { /* Hit */
    return bucketIndex;
  } else { /* Key not in map */
    /* TODO: How should this be handled?
     * TODO: I can't just return null through the array index */
    return -100000000; /* Should crash with SIGSEGV */
  }
}

/* Finds an appropriate bucket for a new entry with a given key.
 * If the key is already present, that keys bucket is returned otherwise an empty one is.
 * Returns the index of the bucket. */
int ds_map_alloc_bucket(void * map, char * key, size_t val_len) {
  DS_MapHeader * mapHeader = ds_map_header(map);

  unsigned long bucketIndex = ds_strhash((unsigned char *) key) % mapHeader->cap;

  DS_MapBucketHeader * bucketHeader = ds_map_bucket_header(map, bucketIndex, val_len);
  /* Linear probing, keys are unique */
  while (bucketHeader->full && strcmp(bucketHeader->key, key)) {
    bucketIndex++;
    bucketIndex %= mapHeader->cap;
    bucketHeader = ds_map_bucket_header(map, bucketIndex, val_len);
  }
  mapHeader->size++;
  bucketHeader->full = 1;
  bucketHeader->key = key;
  if (bucketHeader->tombstone) {
    mapHeader->tombstone_count--;
  }
  bucketHeader->tombstone = 0;
  return bucketIndex;
}

/* Grows a map to a new capacity by allocating a new map and rehashing all entries.
 * Does nothing if the new_cap is equal to the current one. */
void * ds_map_grow_internal(void * old_map, size_t new_cap, size_t val_len) {
  DS_MapHeader * old_header = ds_map_header(old_map);
  if (old_header->cap == new_cap) return old_map;

  /* Allocate the new map */
  size_t new_mem_len = ds_map_mem_len(new_cap, val_len);
  DS_MapHeader * new_header = (DS_MapHeader *) malloc(new_mem_len);
  memset(new_header, 0, new_mem_len);
  new_header->cap = new_cap;
  new_header->size = 0;
  new_header->tombstone_count = 0;

  /* Rehash all entries and copy their values over */
  void * new_map = (char *) new_header + sizeof(DS_MapHeader);
  DS_MapBucketHeader * old_bucket_header;
  void * old_bucket_value;
  void * new_bucket_value;
  size_t old_idx;
  for (old_idx = 0; old_idx < old_header->cap; old_idx++) {
    old_bucket_header = ds_map_bucket_header(old_map, old_idx, val_len);
    if (old_bucket_header->full) {
      old_bucket_value = (char *) old_bucket_header + sizeof(DS_MapBucketHeader);
      size_t new_idx = ds_map_alloc_bucket(new_map, old_bucket_header->key, val_len);
      new_bucket_value = (char *) ds_map_bucket_header(new_map, new_idx, val_len) + sizeof(DS_MapBucketHeader);
      memcpy(new_bucket_value, old_bucket_value, val_len);
    }
  }
  free(old_header);

  return new_map;
}

/* Frees the memory used by a map.
 * Since the key strings are not stored inside the map, they are not freed */
void ds_mapfree(void * map) {
  DS_MapHeader * header = ds_map_header(map);
  free(header);
}

