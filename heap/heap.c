#include "heap.h"

#include <stdlib.h>
#include <string.h>
#include <assert.h>

/* NOT EXPOSED */

static inline void *item_at(
  __ds_heap_struct_t *heap, size_t val_len, int32_t index
) {
  assert(index < ds_vec_len(heap->vector));
  return ((char *) heap->vector) + val_len * index;
}

static inline void swap(
  __ds_heap_struct_t *heap, size_t val_len, int32_t index_a, int32_t index_b
) {
  char temp_buffer[val_len];
  void *loc_a = item_at(heap, val_len, index_a);
  void *loc_b = item_at(heap, val_len, index_b);
  memcpy(temp_buffer, loc_a, val_len);
  memcpy(loc_a, loc_b, val_len);
  memcpy(loc_b, temp_buffer, val_len);
}

static inline int32_t parent_index(int32_t child_index) {
  if (child_index == 0) {
    return -1;
  } else {
    return (child_index - 1) / 2;
  }
}

static inline int32_t left_child_index(int32_t parent_index, void *vector) {
  int32_t index = 2 * parent_index + 1;
  if (index >= ds_vec_len(vector)) {
    return -1;
  } else {
    return index;
  }
}

static inline int32_t right_child_index(int32_t parent_index, void *vector) {
  int32_t index = 2 * parent_index + 2;
  if (index >= ds_vec_len(vector)) {
    return -1;
  } else {
    return index;
  }
}

/* INTERNAL */
void *__ds_heap_create(
  size_t val_len, ds_heap_is_lower_than_func_t *is_lower_than
) {
  __ds_heap_struct_t *heap = malloc(sizeof(__ds_heap_struct_t));
  *heap = (__ds_heap_struct_t){
    .is_lower_than = is_lower_than, .vector = __ds_vec_create(val_len, 4)};
  return &heap->vector;
}

void __ds_heap_heapify_up(
  void *heap_vector, size_t val_len, int32_t start_index
) {
  __ds_heap_struct_t *heap =
    ds_container_of(heap_vector, __ds_heap_struct_t, vector);
  int32_t curr = start_index;
  int32_t curr_parent = parent_index(curr);
  while (curr_parent >= 0) {
    if (heap->is_lower_than(
          item_at(heap, val_len, curr), item_at(heap, val_len, curr_parent)
        )) {
      // Correct order
      break;
    } else {
      // Wrong order, swap
      swap(heap, val_len, curr, curr_parent);
    }
    curr = curr_parent;
    curr_parent = parent_index(curr);
  }
}

void __ds_heap_heapify_down(void *heap_vector, size_t val_len) {
  __ds_heap_struct_t *heap =
    ds_container_of(heap_vector, __ds_heap_struct_t, vector);
  if (ds_vec_len(heap->vector) <= 1) {
    return;
  }

  int32_t curr = 0;
  while (true) {
    int32_t left_child = left_child_index(curr, heap->vector);
    int32_t right_child = right_child_index(curr, heap->vector);

    // Find max of curr and its children
    int32_t largest = curr;
    if (left_child >= 0 && heap->is_lower_than(item_at(heap, val_len, largest), item_at(heap, val_len, left_child))) {
      largest = left_child;
    }
    if (right_child >= 0 && heap->is_lower_than(item_at(heap, val_len, largest), item_at(heap, val_len, right_child))) {
      largest = right_child;
    }
    if (largest != curr) {
      swap(heap, val_len, curr, largest);
      curr = largest;
    } else {
      break;
    }
  }
}

/* EXTERNAL */

bool ds_heap_is_empty(void *heap) { return (ds_vec_len(*(void **) heap)) == 0; }

void ds_heap_free(void *heap) {
  __ds_heap_struct_t *heap_struct =
    ds_container_of(heap, __ds_heap_struct_t, vector);
  vec_free(heap_struct->vector);
  free(heap_struct);
}
