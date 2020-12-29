#pragma once

// Flag this header as a system header. The compiler can generate calls to
// memset and get itself into a silly situation of warning about unused
// functions that cannot be removed due to subsequent linker errors.
#pragma GCC system_header

static void* memset(void* dest, int c, unsigned int n) {
  unsigned char* o = dest;
  unsigned char* const end = o + n;
  while (o != end) *o++ = c;
  return dest;
}
