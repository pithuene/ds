#ifndef DS_UNIQUE_SYMBOL_H
#define DS_UNIQUE_SYMBOL_H

#ifndef DS_NO_SHORT_NAMES
#define unique_symbol ds_unique_symbol
#endif /* DS_NO_SHORT_NAMES */

#define __ds_unique_symbol_tokenpaste(x, y) x##y
#define __ds_unique_symbol_expand(x, y)     __ds_unique_symbol_tokenpaste(x, y)

// This macro is used to generate unique symbols based on the current line in
// macro definitions.
// If you are defining a symbol in a macro, you need to make sure it doesn't
// collide with any existing ones, which is especially tricky when the macro
// os itself used multiple times.
// To lower the chance of name collisions, this macro appends the current line
// number to a given prefix.
// Note, that created symbols can not practially be accessed outside the
// creating macro as you would need to know the calling line number.
#define ds_unique_symbol(PREFIX) __ds_unique_symbol_expand(PREFIX, __LINE__)

#endif  // DS_UNIQUE_SYMBOL_H
