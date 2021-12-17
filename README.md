ds
==

A collection of generic C data structures.

vec
---

A dynamic array. Doubles in size when capacity is exceeded.
Inspired by [Sean Barrett's stb](https://github.com/nothings/stb), metadata (length and capacity) is stored in front of the pointer, making the vector compatible with static C arrays.

``` c
/* Create an int vector with an initial capacity of 10 */
vec_t(int) vector = vec(int, 10);

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

map
---

A dynamically growing hash map using linear probing.
Maps strings to some other type.

``` c
/* Create an map from string to int with an initial capacity of 4 */
map_t(int) hashmap = map(int, 4);

/* Put 42 at key "key1" */
mapput(hashmap, "key1", 42);

/* Get map entry for key "key1" */
int i = mapget(hashmap, "key1");

/* Loop over all map entries */
mapforeach(char * key, int value, hashmap) {
  /* This is called for each key-value pair */
}

/* Free map */
mapfree(vector);
```

linalloc
--------

A linear allocator, useful for temporary data with known maximum size.
Allocates from a single contiguous block of memory.
Freeing can only occur in reverse order of allocation or for the buffer.

``` c
/* Create a new linear allocator "heapAlloc" with a capacity of 128 bytes on the heap */
void * buf = malloc(128);
linear_allocator_t heapAlloc = new_linear_allocator(buf);

/* Create a new linear allocator "stackAlloc" with a capacity of 128 bytes on the stack */
char buf[128];
linear_allocator_t stackAlloc = new_linear_allocator(buf);

/* Allocate a new instance of "Struct" using the "heapAlloc" allocator */
Struct * struc = linalloc(&heapAlloc, sizeof(Struct));

/* Free the allocated instance */
linfree(&heapAlloc, struc);

/* Free an entire buffer used by "heapAlloc" */
free(buf);
```

arenaalloc
----------

An arena allocator, useful for temporary data with unknown maximum size.
Allocates multiple containers which are each used as a linear allocator.

``` c
/* Create a new arena allocator with a container size of 128 bytes */
arena_allocator_t arena = new_arena_allocator(128);

/* Allocate some memory */
long * l1 = arenaalloc(&arena, sizeof(long));

/* Free the entire arena */
free_arena_allocator(&arena);
```

poolalloc
---------

A pool allocator, useful for quickly allocating and freeing fixed size blocks.

``` c
/* Create a new pool allocator */
pool_allocator_t pool = new_pool_allocator(sizeof(long));

/* Allocate some memory */
long * l1 = poolalloc(&pool);

/* Free a block */
poolfree(&pool, l1);

/* Free the entire pool */
free_pool_allocator(&pool);
```

