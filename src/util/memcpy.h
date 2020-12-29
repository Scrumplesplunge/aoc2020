#pragma once

static void* memcpy(void* restrict dest, const void* restrict src,
                    unsigned int n) {
  char* o = dest;
  char* const end = o + n;
  const char* i = src;
  while (o != end) *o++ = *i++;
  return dest;
}
