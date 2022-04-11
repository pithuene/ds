#define _DEFAULT_SOURCE
#include "arenaalloc.h"

/* The number of bytes already allocated from a container */
size_t * arenaContainerSize(void * cont) {
  return (size_t *)(((char *) cont) - sizeof(size_t));
}

void * arenaAllocFromContainer(void * cont, size_t size) {
  size_t * contSize = arenaContainerSize(cont);
  char * result = ((char *) cont) + *contSize;
  *contSize += size;
  return result;
}

arena_allocator_t new_arena_allocator(size_t container_size) {
  arena_allocator_t arena;
  arena.container_size = container_size;
  arena.containers = vec(void *, 2);
  return arena;
}

void * arenaalloc(arena_allocator_t * allocator, size_t size) {
  if (size > allocator->container_size) return NULL;
  size_t i;
  for (i = 0; i < veclen(allocator->containers); i++) {
    if (allocator->container_size - *arenaContainerSize(allocator->containers[i]) >= size) break;
  }
  if (i >= veclen(allocator->containers)) {
    size_t mappingSize = sizeof(size_t) + allocator->container_size;
    size_t * newContainer = mmap(
      NULL,
      mappingSize,
      PROT_READ | PROT_WRITE,
      MAP_PRIVATE | MAP_ANONYMOUS, 0, 0
    );
    *newContainer = 0;
    vecpush(allocator->containers, (void *) ((char *) newContainer + sizeof(size_t)));
  }
  return arenaAllocFromContainer(allocator->containers[i], size);
}

void free_arena_allocator(arena_allocator_t * allocator) {
  size_t i;
  for (i = 0; i < veclen(allocator->containers); i++) {
    size_t mappingSize = sizeof(size_t) + allocator->container_size;
    munmap((void *) arenaContainerSize(allocator->containers[i]), mappingSize);
  }
  vecfree(allocator->containers);
}

void arena_allocator_reset(arena_allocator_t * allocator) {
  size_t number_of_blocks = veclen(allocator->containers);
  for (size_t i = 0; i < number_of_blocks; i++) {
    size_t mappingSize = sizeof(size_t) + allocator->container_size;
    munmap((void *) arenaContainerSize(allocator->containers[i]), mappingSize);
  }
  for (size_t i = 0; i < number_of_blocks; i++) {
    void *removed = vecpop(allocator->containers);
    (void) removed; // Unused, but otherwise the result of vecpop is unused
  }
}
