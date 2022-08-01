#ifndef DS_CONTAINER_OF_H
#define DS_CONTAINER_OF_H

#include <stddef.h>

#ifndef DS_NO_SHORT_NAMES
#define container_of ds_container_of
#endif /* DS_NO_SHORT_NAMES */

// Given a pointer MEMBER_PTR to the member MEMBER_NAME of a struct
// of type CONTAINER_T, return a pointer to the containing struct itself.
//
// Example:
//
// typedef struct {
//   int a;
//   int b;
//   int c;
// } Struct;
// Struct s;
// int *b_ptr = &s.b;
//
// Struct *s_ptr = container_of(b_ptr, Struct, b);
#define ds_container_of(MEMBER_PTR, CONTAINER_T, MEMBER_NAME) \
  ((CONTAINER_T                                               \
      *) (((char *) (MEMBER_PTR)) - offsetof(CONTAINER_T, MEMBER_NAME)))

#endif  // DS_CONTAINER_OF_H
