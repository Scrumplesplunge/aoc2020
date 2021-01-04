#pragma once

// Returns the length of a null-terminated string.
static size_t strlen(const char* c_string) {
  const char* i = c_string;
  while (*i) i++;
  return i - c_string;
}
