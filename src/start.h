// This header is an auto-included prelude for each solver. It provides nothing
// which isn't found basically by default in a normal hosted environment.

// Flag this header as a system header. This will prevent warnings for unused
// statics, as well as for noreturn functions which GCC thinks can return.
#pragma GCC system_header

#define STDIN_FILENO 0u
#define STDOUT_FILENO 1u
#define STDERR_FILENO 2u
#define NULL ((void*)0)

typedef unsigned int size_t;
typedef int ssize_t;
#define bool _Bool
#define true ((_Bool)1)
#define false ((_Bool)0)

// System calls. We will only use three: read, write, and exit.

__attribute__((access (write_only, 2)))
static ssize_t read(unsigned int fd, void* buffer, size_t size) {
  ssize_t result;
  asm volatile("int $0x80"
               : "=a"(result)
               : "a"(3), "b"(fd), "c"(buffer), "d"(size)
               : "memory");
  return result;
}

__attribute__((access (read_only, 2)))
static ssize_t write(unsigned int fd, const void* buffer, size_t size) {
  ssize_t result;
  asm volatile("int $0x80"
               : "=a"(result)
               : "a"(4), "b"(fd), "c"(buffer), "d"(size)
               : "memory");
  return result;
}

static __attribute__((noreturn)) void exit(int code) {
  asm volatile("int $0x80" : : "a"(1), "b"(code));
}

// Entry point. We will invoke main from _start.

static int main();

__attribute__((force_align_arg_pointer))
__attribute__((noreturn)) void _start() {
  // Upon ELF entry, the top of the stack is argc, argv[0], argv[1], etc.
  // However, we can't easily access that from here since the compiler may
  // or may not insert function prelude that adjusts the stack pointer.
  exit(main());
}

// The compiler can automatically generate calls to these functions, so we must
// unconditionally include definitions for them.
#include "util/memcpy.h"
#include "util/memmove.h"
#include "util/memset.h"
