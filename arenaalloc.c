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

ArenaAllocator * newArenaAllocator(size_t container_size) {
  ArenaAllocator * arena = (ArenaAllocator *) malloc(sizeof(ArenaAllocator));
  arena->container_size = container_size;
  arena->containers = vec(void *, 2);
  return arena;
}

void * arenaalloc(ArenaAllocator * allocator, size_t size) {
  if (size > allocator->container_size) return NULL;
  int i;
  for (i = 0; i < veclen(allocator->containers); i++) {
    if (allocator->container_size - *arenaContainerSize(allocator->containers[i]) >= size) break;
  }
  if (i >= veclen(allocator->containers)) {
    size_t * newContainer = malloc(sizeof(size_t) + allocator->container_size);
    *newContainer = 0;
    vecpush(allocator->containers, (void *) ((char *) newContainer + sizeof(size_t)));
  }
  return arenaAllocFromContainer(allocator->containers[i], size);
}


void arenafree(ArenaAllocator * allocator) {
  int i;
  for (i = 0; i < veclen(allocator->containers); i++) {
    free((void *) arenaContainerSize(allocator->containers[i]));
  }
  vecfree(allocator->containers);
  free(allocator);
}
