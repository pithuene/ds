#ifndef DS_LLIST_H
#define DS_LLIST_H

#include "vec.h"
#include <stdint.h>

#define llist        ds_llist
#define llist_t      ds_llist_t
#define llist_item_t ds_llist_item_t
#define llistpush    ds_llistpush
#define llisthead    ds_llisthead
#define llistnext    ds_llistnext
#define llistfree    ds_llistfree

#define ds_llist_t(TYPE) vec_t(ds_llist_item_struct(TYPE)*) *

typedef struct {
  uint32_t null     :  1;
  uint32_t memblock : 31;
  uint32_t offset;
} DS_LList_Item_Ptr;

typedef struct {
  DS_LList_Item_Ptr next;
} DS_LList_Item_Header;

#define ds_llist_item_struct(TYPE) struct { \
  DS_LList_Item_Ptr next; \
  TYPE value; \
}

#define ds_llist_item_t(TYPE) TYPE *

DS_LList_Item_Header * ds_llist_item_header(void * item);

typedef struct {
  /* The numer of items */
  size_t length;

  /* Size of memory blocks */
  size_t memblock_size;
  /* Pointer to the next free slot, NULL if there are none */
  DS_LList_Item_Ptr free_list;
  DS_LList_Item_Ptr head;
  DS_LList_Item_Ptr tail;
} DS_LListHeader;

/** Utils **/
DS_LListHeader * ds_llist_header(void * v);

/* Get the next item from a given llist_item_t */
#define ds_llistnext(LIST, ITEM) ds_llist_get(LIST, ds_llist_item_header(ITEM)->next)

/** New Linked List **/
#define ds_llist(TYPE, BLOCK_SIZE) (ds_llist_new(sizeof(TYPE), BLOCK_SIZE))

/* Create a new list with values of length val_len and memory_blocks that each fit block_size of these values */
void * ds_llist_new(size_t val_len, size_t block_size);

/** Access list **/
DS_LList_Item_Ptr ds_llisthead_ptr(void * l);
DS_LList_Item_Ptr ds_llisttail_ptr(void * l);

#define ds_llist_get(LLIST, PTR) ((PTR.null) ? (void *)0 : &(*LLIST)[PTR.memblock][PTR.offset].value)

#define ds_llisthead(LLIST) (ds_llist_get(LLIST, ds_llisthead_ptr(LLIST)))
#define ds_llisttail(LLIST) (ds_llist_get(LLIST, ds_llisttail_ptr(LLIST)))


/** Insert into linked list **/

DS_LList_Item_Ptr ds_llist_alloc_item(void * l, size_t entry_len);

void ds_llist_add_memblock(void * l, size_t entry_len);

#define ds_llist_entry_size(LLIST) sizeof(***LLIST)

#define ds_llistpush(LLIST, VAL) \
  for (DS_LList_Item_Ptr item = ds_llist_alloc_item(LLIST, ds_llist_entry_size(LLIST)), *_=(void*)1; _; _=(void*)0) \
    for (DS_LListHeader * header = ds_llist_header(LLIST), *_=(void*)1; _; _=(void*)0) { \
      (*LLIST)[item.memblock][item.offset].value = VAL; \
      (*LLIST)[item.memblock][item.offset].next.null = 1; \
      if (!header->tail.null) ds_llist_item_header(ds_llist_get(LLIST, header->tail))->next = item;  \
      header->tail = item; \
      if (header->length == 0) header->head = item; \
      header->length++; \
    }
  
/** Free list **/
void ds_llistfree(void * l);

#endif
