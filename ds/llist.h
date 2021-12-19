#ifndef DS_LLIST_H
#define DS_LLIST_H

#include "vec.h"
#include <stdint.h>

#define llist_t(TYPE) vec_t(ds_llist_item_t(TYPE)*) *

#define llist ds_llist
#define llist_item_t ds_llist_item_t
#define llistpush ds_llistpush
#define llisthead ds_llisthead
#define llistnext ds_llistnext
#define llistfree ds_llistfree

typedef struct {
  uint32_t null     :  1;
  uint32_t memblock : 31;
  uint32_t offset;
} DS_LList_Item_Ptr;

/* TODO: To create an elegant API, it would probably best if list_item_t(int) were just an int * and the next pointer would be stored in front of this, which the llistnext function retrieves */
#define ds_llist_item_t(TYPE) struct { \
  DS_LList_Item_Ptr next; \
  TYPE value; \
}

typedef struct {
  /* The numer of items */
  size_t length;

  /* Size of memory blocks */
  size_t memblock_size;
  /* Pointer to the next free slot, NULL if there are none */
  DS_LList_Item_Ptr free_list;
  DS_LList_Item_Ptr head;
} DS_LListHeader;

/** Utils **/
DS_LListHeader * ds_llist_header(void * v);

/** New Linked List **/
#define ds_llist(TYPE, BLOCK_SIZE) (ds_llist_new(sizeof(TYPE), BLOCK_SIZE))

/* Create a new list with values of length val_len and memory_blocks that each fit block_size of these values */
void * ds_llist_new(size_t val_len, size_t block_size);

/** Access list **/
DS_LList_Item_Ptr ds_llisthead_ptr(void * l);

#define ds_llist_get(LLIST, PTR) (&(*LLIST)[PTR.memblock][PTR.offset])

#define ds_llisthead(LLIST) (ds_llist_get(LLIST, ds_llisthead_ptr(LLIST)))

/* Get the next item from a given llist_item_t */
#define ds_llistnext(LLIST, ITEM) (ds_llist_get(LLIST, ITEM->next))

/** Insert into linked list **/

DS_LList_Item_Ptr ds_llist_alloc_item(void * l, size_t entry_len);

void ds_llist_add_memblock(void * l, size_t entry_len);

#define ds_llist_entry_size(LLIST) sizeof(***LLIST)

#define ds_llistpush(LLIST, VAL) \
  for (DS_LList_Item_Ptr item = ds_llist_alloc_item(LLIST, ds_llist_entry_size(LLIST)), *_=(void*)1; _; _=(void*)0) \
  (*LLIST)[item.memblock][item.offset].value = VAL
// TODO: Doesn't set the next pointer of the previous value yet

/** Free list **/
void ds_llistfree(void * l);

#endif
