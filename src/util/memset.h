#pragma once

// Flag this header as a system header. The compiler can generate calls to
// memset and get itself into a silly situation of warning about unused
// functions that cannot be removed due to subsequent linker errors.
#pragma GCC system_header

// Fill n bytes of memory starting at dest with copies of c.
static void* memset(void* dest, int c, size_t n) {
  unsigned char* o = dest;
  unsigned char* const end = o + n;
  while (o != end) *o++ = c;
  return dest;
}
