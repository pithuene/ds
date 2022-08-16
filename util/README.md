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

`unique_symbol`
--------------

This macro is used to generate unique symbols based on the current line in
macro definitions.
If you are defining a symbol in a macro, you need to make sure it doesn't
collide with any existing ones, which is especially tricky when the macro
os itself used multiple times.
To lower the chance of name collisions, this macro appends the current line
number to a given prefix.
Note, that created symbols can not practially be accessed outside the
creating macro as you would need to know the calling line number.
