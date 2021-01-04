#pragma once

// Copy n bytes from src to dest. Return dest. The src and dest regions must not
// overlap.
static void* memcpy(void* restrict dest, const void* restrict src, size_t n) {
  char* o = dest;
  char* const end = o + n;
  const char* i = src;
  while (o != end) *o++ = *i++;
  return dest;
}
