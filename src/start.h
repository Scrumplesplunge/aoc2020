// This header is an auto-included prelude for each solver. It provides nothing
// which isn't found basically by default in a normal hosted environment.

// Flag this header as a system header. This will prevent warnings for unused
// statics, as well as for noreturn functions which GCC thinks can return.
#pragma GCC system_header

#define STDIN_FILENO 0u
#define STDOUT_FILENO 1u
#define STDERR_FILENO 2u
#define NULL ((void*)0)

typedef unsigned long size_t;
typedef long ssize_t;

// System calls. We will only use three: read, write, and exit.

static ssize_t read(unsigned int fd, const void* buffer, size_t size) {
  ssize_t result;
  asm volatile("syscall"
               : "=a"(result)
               : "a"(0), "D"(fd), "S"(buffer), "d"(size)
               : "cc", "rcx", "r11", "memory");
  return result;
}

static ssize_t write(unsigned int fd, const void* buffer, size_t size) {
  ssize_t result;
  asm volatile("syscall"
               : "=a"(result)
               : "a"(1), "D"(fd), "S"(buffer), "d"(size)
               : "cc", "rcx", "r11", "memory");
  return result;
}

static __attribute__((noreturn)) void exit(int code) {
  asm volatile("syscall" : : "a"(60), "D"((long)code));
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
