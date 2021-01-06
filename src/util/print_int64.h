#pragma once

// Print an integer in decimal, followed by a newline.
static void print_int64(unsigned long long x) {
  unsigned char buffer[24] = {[23] = '\n'};
  // Compute the decimal format one bit at a time by doubling and carrying BCD.
  for (int i = 60; i >= 0; i -= 4) {
    for (int j = 0; j < 23; j++) buffer[j] *= 16;
    unsigned char carry = (x >> i) & 0xF;
    for (int j = 22; j >= 0; j--) {
      unsigned char temp = buffer[j] + carry;
      buffer[j] = temp % 10;
      carry = temp / 10;
    }
  }
  // Compute the most significant digit and truncate the output.
  int i = 0;
  while (buffer[i] == 0) i++;
  const int start = i < 22 ? i : 22;
  for (int j = start; j < 23; j++) buffer[j] += '0';
  write(STDOUT_FILENO, buffer + start, 24 - start);
}
