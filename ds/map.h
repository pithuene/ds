#ifndef DS_MAP_H
#define DS_MAP_H

#include <stdlib.h>
#include <string.h>

#define map_t(TYPE) DS_MapBucket(TYPE) *

#define map ds_map
#define mapput ds_mapput
#define mapget ds_mapget
#define mapfree ds_mapfree
#define mapforeach ds_mapforeach

typedef struct {
  char * key;
  char full : 1;
  char tombstone : 1;
} DS_MapBucketHeader;

#define DS_MapBucket(TYPE) struct { \
  DS_MapBucketHeader header; \
  TYPE value; \
}

typedef struct {
  /* The numer of filled buckets */
  size_t size;
  /* The numer of buckets */
  size_t cap;
  /* The number of tombstones in the map */
  size_t tombstone_count;
} DS_MapHeader;

/** Utils **/
DS_MapHeader * ds_map_header(void * v);

/** New Map **/
#define ds_map(TYPE, CAP) (ds_map_new(sizeof(TYPE), CAP))

void * ds_map_new(size_t val_len, size_t cap);

/** Map Put **/
size_t ds_map_grow_capacity(void * map, size_t new_elem_count);

void * ds_map_grow_internal(void * map, size_t new_cap, size_t val_len);

#define ds_map_grow(MAP, NEWCAP) \
  ((MAP) = ds_map_grow_internal(MAP, NEWCAP, sizeof(MAP->value)))

#define ds_map_ensure_capacity(MAP, COUNT) \
  (ds_map_grow(MAP, ds_map_grow_capacity(MAP, COUNT)))

int ds_map_alloc_bucket(void * map, char * key, size_t val_len);

/* TODO: Should VALUE be TYPE or TYPE * here? */
#define ds_mapput(MAP, KEY, VALUE) ( \
  ds_map_ensure_capacity(MAP, 1), \
  (MAP)[ds_map_alloc_bucket(MAP, KEY, sizeof(VALUE))].value = (VALUE))

/** Map Get **/
int ds_map_get_key_idx(void * map, char * key, size_t val_len);

#define ds_mapget(MAP, KEY) \
  (MAP[ds_map_get_key_idx(MAP, KEY, sizeof(MAP->value))].value)

/** Free Map **/
void ds_mapfree(void * map);

/** Foreach **/
/* TODO: Would it be more efficient to move the key and value declarations to the outside of the loop? */
/* TODO: It might make sense to offer a mutable version too (value needs to be set using mapput currently)*/

/* Iterate over each entry of MAP.
 * 
 * map_t(int) intmap = map(int, 4);
 * ...
 * mapforeach(char * key, int value, intmap) { ... }
 */
#define ds_mapforeach(KEY_DECL, VAL_DECL, MAP) \
  for (int ds_map_foreach_i = 0; ds_map_foreach_i < ds_map_header(MAP)->cap; ds_map_foreach_i++) /* Iterate over all buckets, full or not */ \
  for (DS_MapBucketHeader *header = &MAP[ds_map_foreach_i].header, *_=(void*)1; header->full && _; _=(void*)0) /* Do this only if the bucket is full, but once per bucket at most */ \
  for (KEY_DECL = header->key, *_=(void*)1; _; _=(void*)0) /* Declare key */\
  for (VAL_DECL = MAP[ds_map_foreach_i].value, *_=(void*)1; _; _=(void*)0) /* Declare value */

#endif
