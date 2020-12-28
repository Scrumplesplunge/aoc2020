#pragma once

// GCC automatically generates calls to __udivdi3 and __umoddi3 when attempting
// division with `unsigned long long` variables, so we need to provide
// a definition. However, it seems that these magic symbols don't get GC'd
// properly when unused, so uncommenting this code causes all solvers to get
// bigger.

struct divmod_result {
  unsigned long long quotient;
  unsigned long long remainder;
};

static __attribute__((pure)) struct divmod_result divmod(unsigned long long a,
                                                         unsigned long long b) {
  unsigned long long accumulator = 0;
  unsigned long long value = b;
  unsigned long long factor = 1;
  while ((value << 1) <= a) {
    value <<= 1;
    factor <<= 1;
  }
  while (factor) {
    if (value <= a) {
      a -= value;
      accumulator += factor;
    }
    value >>= 1;
    factor >>= 1;
  }
  return (struct divmod_result){.quotient = accumulator, .remainder = a};
}

__attribute__((pure)) unsigned long long __udivdi3(unsigned long long a,
                                                   unsigned long long b) {
  return divmod(a, b).quotient;
}

__attribute__((pure)) unsigned long long __umoddi3(unsigned long long a,
                                                   unsigned long long b) {
  return divmod(a, b).remainder;
}
