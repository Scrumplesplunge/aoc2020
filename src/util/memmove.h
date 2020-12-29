#pragma once

static void* memmove(void* dest, const void* src,
                    unsigned int n) {
  if (dest < src) {
    char* o = dest;
    char* const end = o + n;
    const char* i = src;
    while (o != end) *o++ = *i++;
    return dest;
  } else {
    char* const end = dest;
    char* o = end + n;
    const char* i = src + n;
    do {
      *--o = *--i;
    } while (o != end);
    return dest;
  }
}
