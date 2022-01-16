#include "llist.h"
#include <assert.h>
#include <stdio.h>

int main(void) {
  llist_t(int) my_list = llist(int, 100);
  // llist_t(char *) str_list = llist(int, 100); TODO: Why is this ok?

  for (int i = 0; i < 1000; i++) {
    llistpush(my_list, i);
  }

  DS_LListHeader * header = ds_llist_header(my_list);

  printf("length: %d\n", header->length);
  printf("memblock_size: %d\n", header->memblock_size);
  printf("memblock_len: %d\n", veclen(*my_list));
  printf("free_list: %s %d/%d\n", header->free_list.null ? "NULL" : "", header->free_list.memblock, header->free_list.offset);
  printf("head: %s %d/%d\n", header->head.null ? "NULL" : "", header->head.memblock, header->head.offset);
  printf("tail: %s %d/%d\n", header->tail.null ? "NULL" : "", header->tail.memblock, header->tail.offset);

  llist_item_t(int) curr = llisthead(my_list);
  int expected = 0;
  while (curr) {
    //printf("%d\n", *curr);
    assert(*curr == expected);
    expected++;
    curr = llistnext(my_list, curr);
  }
  
  // llist_item_t(int) item = llisthead(my_list);
  // llist_item_t(int) next = llistnext(my_list, item);

  llistfree(my_list);
  return 0;
}
