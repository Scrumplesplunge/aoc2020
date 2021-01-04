#pragma once

// Print an integer in decimal, followed by a newline.
static void print_int64(unsigned long long x) {
  char buffer[24];
  buffer[23] = '\n';
  int i = 23;
  do {
    --i;
    buffer[i] = '0' + (x % 10);
    x /= 10;
  } while (x);
  write(STDOUT_FILENO, buffer + i, 24 - i);
}
