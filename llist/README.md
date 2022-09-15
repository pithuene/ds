Linked List
===========

A generic linked list.

Usage
-----

Create a linked list of integers.
``` c
llist_t(int) list = llist_create(int);
```

Append an element to the list.
``` c
llist_append(list, 123);
```

Prepend an element to the list.
``` c
llist_prepend(list, 123);
```

Get the first element of the list
``` c
llist_item_t(int) item = llist_first(list);
```

Get the last element of the list
``` c
llist_item_t(int) item = llist_last(list);
```

Append an item after another one
``` c
llist_append_after(list, item, 1234);
```

Free all items of a list and the list itself
``` c
llist_free(list);
```
