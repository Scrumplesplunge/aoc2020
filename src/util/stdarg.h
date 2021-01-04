#ifndef STDARG_H_
#define STDARG_H_

typedef __builtin_va_list va_list;
#define va_start(list, param) __builtin_va_start((list), (param))
#define va_arg(list, type) __builtin_va_arg((list), type)
#define va_end(list) __builtin_va_end((list))
#define va_copy(dest, src) __builtin_va_copy((dest), (src))

// static char* _va_arg(va_list* list, unsigned size) {
//   // Arguments smaller than a word are aligned to a word.
//   size = size < 4 ? 4 : size;
//   char* temp = (char*)(((unsigned)*list + 3) & ~3);
//   *list = temp + size;
//   return temp;
// }
// 
// #define va_start(list, param) ((list) = ((char*)&(param)) + sizeof (param))
// #define va_arg(list, type)  \
//   (*(type*)_va_arg(&(list), sizeof (type)))
// #define va_end(list)
// #define va_copy(dest, src) ((dest) = (src))

#endif  // STDARG_H_
