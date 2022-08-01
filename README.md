ds
==

Generic typesafe datastructures for C99 and up.

Datastructures
--------------

- `arr_t(T)` — A static array with runtime known length
- [`vec_t(T)`](./vec/README.md) — A dynamic array
- [`set_t(T)`](./set/README.md) — A hash set
- [`map_t(K, V)`](./map/README.md) — A hash map with typesafe values
- [`pool_allocator_t(T)`](./mem/pool/README.md) — A generic memory pool

There are also various utilities in [/util](./util/README.md).

Design and alternatives
-----------------------

This library implements most operations using macros which perform various index / pointer calculations behind the scenes and eventually perform typesafe array / pointer accesses.
The only other library I found using this approach is [stb_ds](http://nothings.org/stb_ds/) by Sean Barrett, which is also what initially inspired me to build this.

Most other libraries for generic typesafe datastructures in C use the preprocessor to instantiate implementations of specific datastructure and type parameter combinations.
An example of this is [tylov/STC](https://github.com/tylov/STC).
The downsides of that method are, that the symbols for each instantiation must be unique, so they usually contain the names of the type parameters (think `map_int_long_put` instead of `map_put`), and that the instatiation macro must explicitly be called for every type combintation.
