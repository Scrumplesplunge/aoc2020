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

// System calls. We will only use three: read, write, and exit.

static ssize_t read(unsigned int fd, const void* buffer, size_t size) {
  ssize_t result;
  asm volatile("int $0x80"
               : "=a"(result)
               : "a"(3), "b"(fd), "c"(buffer), "d"(size)
               : "memory");
  return result;
}

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

__attribute__((noreturn)) void _start() {
  int argc;
  char** argv;
  asm volatile("pop %0\n"
               "mov %%esp, %1"
               : "=r"(argc), "=r"(argv));
  exit(main(argc, argv));
}
