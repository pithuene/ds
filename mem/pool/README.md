Generic memory pool
===================

A typesafe pool allocator.
Can be used to manage a large number of elements of the same type while providing fast allocation times and no internal fragmentation.

Usage
-----


Memory layout
-------------

A `pool_allocator_t(T)` is a pointer to an array of `T*`.
In front of that array the `block_idx` and `cell_idx`, which specify the head of the free list, are stored.
Each `T*` in the array points to an array of `T`.

![asdf](./pool_allocator_t.svg)
