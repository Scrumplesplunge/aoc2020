#pragma once

#include "is_digit.h"

// Read a decimal integer from the string at input into value, returning the
// address of the first byte after the integer.
static const char* read_int64(const char* input, unsigned long long* value) {
  if (!is_digit(*input)) die("bad");
  unsigned long long temp = 0;
  while (is_digit(*input)) {
    temp = 10 * temp + (*input - '0');
    input++;
  }
  *value = temp;
  return input;
}
