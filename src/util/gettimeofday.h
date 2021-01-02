#pragma once

// System call for getting timing information.

struct timeval {
  // Current time in seconds.
  unsigned tv_sec;
  // Microseconds.
  unsigned tv_usec;
};

static int gettimeofday(struct timeval* restrict tp, void* restrict tzp) {
  int result;
  asm volatile("int $0x80"
               : "=a"(result)
               : "a"(78), "b"(tp), "c"(tzp)
               : "memory");
  return result;
}
