llist
=====

A singly linked list.

An `llist_t(TYPE)` value is a pointer to a vector of list-item arrays.
This way, no `realloc` is necessary when the capacity is exceeded and there is no need for potentially large contiguous blocks of memory. In the `vec` implementation this was desirable because it allows for regular array-style access, since the items of the linked list are not in any particular order, this is not necessary here.
