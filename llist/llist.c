#include "llist.h"

#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>
#include <assert.h>

#include "../util/container_of.h"

/* NOT EXPOSED */

typedef struct llist_item_struct_t {
  struct llist_item_struct_t *next;
  uint64_t value[];  // T value
} llist_item_struct_t;

typedef struct {
  // The number of elements
  uint64_t size;
  // The head of the linked list
  llist_item_struct_t *first;
  // The tail of the linked list
  llist_item_struct_t *last;
  // Not actually a void *, but a T * used for typesafe access to any memory
  // location
  void *t_ptr;
} llist_struct_t;

/* INTERNAL */

void *__ds_llist_create() {
  llist_struct_t *header = malloc(sizeof(llist_struct_t));
  *header = (llist_struct_t){
    .size = 0,
    .first = NULL,
    .last = NULL,
    .t_ptr = NULL,
  };
  return &header->t_ptr;
}

// Allocate and append a new llist item.
// Set the t_ptr to its value.
void __ds_llist_append(void *llist_t_ptr, size_t val_len) {
  llist_struct_t *llist = ds_container_of(llist_t_ptr, llist_struct_t, t_ptr);
  llist_item_struct_t *new_item = malloc(sizeof(llist_item_struct_t) + val_len);
  new_item->next = NULL;
  if (llist->last) {
    llist->last->next = new_item;
  }
  llist->last = new_item;
  if (llist->first == NULL) llist->first = new_item;

  llist->t_ptr = &new_item->value;
}

// Allocate and prepend a new llist item.
// Set the t_ptr to its value.
void __ds_llist_prepend(void *llist_t_ptr, size_t val_len) {
  llist_struct_t *llist = ds_container_of(llist_t_ptr, llist_struct_t, t_ptr);
  llist_item_struct_t *new_item = malloc(sizeof(llist_item_struct_t) + val_len);
  new_item->next = llist->first;
  if (llist->last == NULL) {
    llist->last = new_item;
  }
  llist->first = new_item;

  llist->t_ptr = &new_item->value;
}

// Allocate a new llist item and append it after a given one.
// Set the t_ptr to the new items value.
void __ds_llist_append_after(
  void *llist_t_ptr, void *reference_item, size_t val_len
) {
  llist_struct_t *llist = ds_container_of(llist_t_ptr, llist_struct_t, t_ptr);
  llist_item_struct_t *new_item = malloc(sizeof(llist_item_struct_t) + val_len);

  if (reference_item == NULL) {
    __ds_llist_prepend(llist_t_ptr, val_len);
  } else {
    llist_item_struct_t *reference =
      ds_container_of(reference_item, llist_item_struct_t, value);

    if (llist->last == reference) {
      llist->last = new_item;
    }
    new_item->next = reference->next;
    reference->next = new_item;

    llist->t_ptr = &new_item->value;
  }
}

void __ds_llist_first(void *list_t_ptr) {
  llist_struct_t *llist = ds_container_of(list_t_ptr, llist_struct_t, t_ptr);
  if (llist->first == NULL) {
    llist->t_ptr = NULL;
  } else {
    llist->t_ptr = &(llist->first->value);
  }
}

void __ds_llist_last(void *list_t_ptr) {
  llist_struct_t *llist = ds_container_of(list_t_ptr, llist_struct_t, t_ptr);
  if (llist->last == NULL) {
    llist->t_ptr = NULL;
  } else {
    llist->t_ptr = &(llist->last->value);
  }
}

void *__ds_llist_next(void *item_value) {
  llist_item_struct_t *item =
    ds_container_of(item_value, llist_item_struct_t, value);
  if (item->next == NULL) {
    return NULL;
  } else {
    return item->next->value;
  }
}

/* EXTERNAL */

void ds_llist_free(void *llist_t_ptr) {
  llist_struct_t *llist = ds_container_of(llist_t_ptr, llist_struct_t, t_ptr);

  llist_item_struct_t *item = llist->first;
  while (item != NULL) {
    llist_item_struct_t *next = item->next;
    free(item);
    item = next;
  }
  free(llist);
}
