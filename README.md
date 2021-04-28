ds
==

A collection of small, simple, header-only ANSI C data structures.

vec
---

A dynamic array. Doubles in size when capacity is exceeded.
Inspired by [Sean Barrett's stb](https://github.com/nothings/stb), metadata (length and capacity) is stored in front of the pointer, making the vector compatible with static C arrays.

``` c
/* Create an int vector with an initial capacity of 10 */
int * vector = vec(int, 10);

/* Append 42 */
vecpush(vector, 42);

/* Access value at index */
vector[i] = 5;
int v = vector[i];

/* Get length */
veclen(vector);

/* Pop last element */
int last = vecpop(vector);

/* Remove ith element */
int el = vecrm(vector, i);

/* Remove ith element but change array order. This is much faster than vecrm. */
int el = vecrmu(vector, i);

/* Free vector */
vecfree(vector);
```
