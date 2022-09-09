#ifndef DS_LLIST_H
#define DS_LLIST_H

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include <stdlib.h>
#include <assert.h>

#ifndef DS_NO_SHORT_NAMES
#define llist_t            ds_llist_t
#define llist_item_t       ds_llist_item_t
#define llist_create       ds_llist_create
#define llist_free         ds_llist_free
#define llist_append       ds_llist_append
#define llist_prepend      ds_llist_prepend
#define llist_append_after ds_llist_append_after
#define llist_first        ds_llist_first
#define llist_last         ds_llist_last
#define llist_next         ds_llist_next
#define llist_foreach      ds_llist_foreach
#define llist_next         ds_llist_next
#endif /* DS_NO_SHORT_NAMES */

/* INTERNAL */

void *__ds_llist_create();

void __ds_llist_append(void *llist_t_ptr, size_t val_len);

void __ds_llist_prepend(void *llist_t_ptr, size_t val_len);

void __ds_llist_append_after(
  void *llist_t_ptr, void *reference_item, size_t val_len
);

// Set the t_ptr to the value of the first item
void __ds_llist_first(void *list_t_ptr);
//
// Set the t_ptr to the value of the last item
void __ds_llist_last(void *list_t_ptr);

void *__ds_llist_next(void *item_value);

/* EXTERNAL */

#define ds_llist_t(T)      T **
#define ds_llist_item_t(T) T *

// Get the ds_llist_item_t of the first item
#define ds_llist_first(LIST) (__ds_llist_first(LIST), (*(LIST)))

// Get the ds_llist_item_t of the last item
#define ds_llist_last(LIST) (__ds_llist_last(LIST), (*(LIST)))

// ds_llist_itemref_t ds_llist_last(void *list);

#define ds_llist_next(LIST, ITEM) (*(LIST) = __ds_llist_next(ITEM))

// TODO: Is this macro necessary?
#define ds_llist_create(TYPE) ((ds_llist_t(TYPE)) __ds_llist_create())

#define ds_llist_append(LIST, VALUE) \
  (__ds_llist_append(LIST, sizeof(**(LIST))), (**(LIST)) = (VALUE))

#define ds_llist_prepend(LIST, VALUE) \
  (__ds_llist_prepend(LIST, sizeof(**(LIST))), (**(LIST)) = (VALUE))

#define ds_llist_append_after(LIST, REF, VALUE) \
  (__ds_llist_append_after(LIST, REF, sizeof(**(LIST))), (**(LIST)) = (VALUE))

#endif  // DS_LLIST_H
