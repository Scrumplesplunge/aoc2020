#ifndef STDARG_H_
#define STDARG_H_

typedef __builtin_va_list va_list;
#define va_start(list, param) __builtin_va_start((list), (param))
#define va_arg(list, type) __builtin_va_arg((list), type)
#define va_end(list) __builtin_va_end((list))
#define va_copy(dest, src) __builtin_va_copy((dest), (src))

#endif  // STDARG_H_
