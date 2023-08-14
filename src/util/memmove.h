#pragma once

// Copy n bytes from src to dest. The src and dest regions may overlap.
void* memmove(void* dest, const void* src, size_t n) {
  if (dest < src) {
    char* o = dest;
    char* const end = o + n;
    const char* i = src;
    while (o != end) *o++ = *i++;
    return dest;
  } else {
    char* const end = dest;
    char* o = end + n;
    const char* i = (const char*)src + n;
    do {
      *--o = *--i;
    } while (o != end);
    return dest;
  }
}
