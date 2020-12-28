#pragma once

#include "is_digit.h"

// Read a decimal integer from the string at input into value, returning the
// address of the first byte after the integer.
static const char* read_int16(const char* input, unsigned short* value) {
  if (!is_digit(*input)) die("bad");
  unsigned short temp = 0;
  while (is_digit(*input)) {
    temp = 10 * temp + (*input - '0');
    input++;
  }
  *value = temp;
  return input;
}
