#include "llist.h"

/* Returns the pointer to the map header stored in front */
DS_LListHeader * ds_llist_header(void * l) {
  return (DS_LListHeader *) ((char *) l - sizeof(DS_LListHeader));
}

DS_LList_Item_Header * ds_llist_item_header(void * item) {
  return (DS_LList_Item_Header *) ((char *) item - sizeof(DS_LList_Item_Header));
}

DS_LList_Item_Ptr ds_llisthead_ptr(void * l) {
  DS_LListHeader * header = ds_llist_header(l);
  return header->head;
}

DS_LList_Item_Ptr ds_llisttail_ptr(void * l) {
  DS_LListHeader * header = ds_llist_header(l);
  return header->tail;
}

/* Allocate a new memblock and add it to the memblock vec.
 * Prepends the memblock to the free list */
void ds_llist_add_memblock(void * l, size_t entry_len) {
  vec_t(void *) * list = l;
  DS_LListHeader * header = ds_llist_header(l);

  // Add new memblock
  void * memblock = malloc(header->memblock_size);
  memset(memblock, 0, header->memblock_size);
  size_t memblock_idx = veclen(*list);
  vecpush(*list, memblock);

  // Add all items in the memblock to the free list
  DS_LList_Item_Ptr next = header->free_list;
  DS_LList_Item_Ptr * entry = NULL;
  for (uint64_t offset = 0; offset < header->memblock_size / entry_len; offset++) {
    entry = (DS_LList_Item_Ptr *) ((char *) memblock + entry_len * offset);
    *entry = next;
    next.null = 0;
    next.memblock = memblock_idx;
    next.offset = offset;
  }
  header->free_list = next;
}

/* Get an empty item slot and mark it as in use
 * entry_len is the size of one ds_llist_item_t(TYPE) */
DS_LList_Item_Ptr ds_llist_alloc_item(void * l, size_t entry_len) {
  vec_t(void *) * list = l;
  DS_LListHeader * header = ds_llist_header(list);

  if (header->free_list.null) {
    ds_llist_add_memblock(list, entry_len);
  }

  // Retrieve the next pointer from the item that will now be used
  DS_LList_Item_Ptr result = header->free_list;
  char * result_memblock = (*list)[result.memblock];
  char * result_entry = result_memblock + entry_len * result.offset;
  DS_LList_Item_Ptr * next = (DS_LList_Item_Ptr *) result_entry;

  // Remove the item from the free list
  header->free_list = *next;

  return result;
}

void * ds_llist_new(size_t val_len, size_t block_size) {
  DS_LListHeader * header = (DS_LListHeader *) malloc(sizeof(DS_LListHeader) + sizeof(void *));
  header->length = 0;
  header->memblock_size = (sizeof(DS_LList_Item_Header) + val_len) * block_size;
  header->free_list.memblock = 0;
  header->free_list.offset = 0;
  header->free_list.null = 1;
  header->head.memblock = 0;
  header->head.offset = 0;
  header->head.null = 1;
  header->tail.memblock = 0;
  header->tail.offset = 0;
  header->tail.null = 1;
  
  vec_t(void *) * vec_ptr = (vec_t(void*)*) ((char *) header + sizeof(DS_LListHeader));
  *vec_ptr = vec(void *, 4);

  return vec_ptr;
}

void ds_llistfree(void * l) {
  vec_t(void *) * list = l;
  DS_LListHeader * header = ds_llist_header(l);
  for (size_t i = 0; i < veclen(*list); i++) {
    free((*list)[i]);
  }
  vecfree(*list);
  free(header);
}
