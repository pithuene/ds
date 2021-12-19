#include "llist.h"
#include <assert.h>
#include <stdio.h>

int main(void) {
  llist_t(int) my_list = llist(int, 100);

  llist_item_t(int) * item = (void *) llisthead(my_list);
  //llist_item_t(int) * next = (void *) llistnext(my_list, item);

  for (int i = 0; i < 1000; i++) {
    llistpush(my_list, i);
  }

  llistfree(my_list);

  return 0;
}
