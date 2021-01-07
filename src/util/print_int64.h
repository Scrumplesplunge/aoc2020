#pragma once

// Print an integer in decimal, followed by a newline.
static void print_int64(unsigned long long x) {
  // Use native BCD support to convert the number to decimal. This will only
  // work for numbers smaller than 2^53, which is sufficient for all puzzles.
  unsigned char bcd[10];
  asm("fbstp %0" : "=m"(bcd) : "t"((double)x) : "st");
  char buffer[19];
  for (int i = 0; i < 9; i++) {
    const unsigned char c = bcd[8 - i];
    buffer[2 * i] = '0' + (c >> 4);
    buffer[2 * i + 1] = '0' + (c & 0xF);
  }
  buffer[18] = '\n';
  int i = 0;
  while (i < 17 && buffer[i] == '0') i++;
  write(STDOUT_FILENO, buffer + i, 19 - i);
}
