#pragma once

static unsigned int strlen(const char* c_string) {
  const char* i = c_string;
  while (*i) i++;
  return i - c_string;
}
