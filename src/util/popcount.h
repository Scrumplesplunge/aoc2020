#pragma once

static int popcount(unsigned int x) {
  int total;
  // This is equivalent to: for (int i = 0; i < 32; i++) total += (x >> i) & 1;
  asm("popcnt %1, %0" : "=r"(total) : "r"(x));
  return total;
}
