#pragma once

struct timeval {
  unsigned tv_sec;
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
