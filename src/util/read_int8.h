#pragma once

#include "is_digit.h"

// Read a decimal integer from the string at input into value, returning the
// address of the first byte after the integer.
static const char* read_int(const char* input, unsigned char* value) {
  if (!is_digit(*input)) die("bad");
  unsigned char temp = 0;
  while (is_digit(*input)) {
    temp = 10 * temp + (*input - '0');
    input++;
  }
  *value = temp;
  return input;
}
