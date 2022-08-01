Utils
=====

Various utilitiy functions or macros.

`container_of`
--------------

Given a pointer to a member of a struct, get a pointer to the containing struct itself.
Used as `container_of(MEMBER_PTR, CONTAINER_T, MEMBER_NAME)` where `MEMBER_PTR` is the pointer to the member, `CONTAINER_T` is the type of the struct and `MEMBER_NAME` is the name of the member.

```c
typedef struct {
  int a;
  int b;
  int c;
} Struct;
Struct s;
int *b_ptr = &s.b;

Struct *s_ptr = container_of(b_ptr, Struct, b);
```
