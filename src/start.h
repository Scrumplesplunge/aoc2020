// This header is an auto-included prelude for each solver. It provides nothing
// which isn't found basically by default in a normal hosted environment.

// Flag this header as a system header. This will prevent warnings for unused
// statics, as well as for noreturn functions which GCC thinks can return.
#pragma GCC system_header

#define STDIN_FILENO 0
#define STDOUT_FILENO 1
#define STDERR_FILENO 2
#define NULL ((void*)0)

// System calls. We will only use three: read, write, and exit.

// extern int read(int fd, const void* buffer, int size);
// extern int write(int fd, const void* buffer, int size);
// extern __attribute__((noreturn)) void exit(int code);

static int read(int fd, const void* buffer, int size) {
  int result;
  asm volatile("int $0x80"
               : "=a"(result)
               : "a"(3), "b"(fd), "c"(buffer), "d"(size)
               : "memory");
  return result;
}

static int write(int fd, const void* buffer, int size) {
  int result;
  asm volatile("int $0x80"
               : "=a"(result)
               : "a"(4), "b"(fd), "c"(buffer), "d"(size)
               : "memory");
  return result;
}

static __attribute__((noreturn)) void exit(int code) {
  asm volatile("int $0x80" : : "a"(1), "b"(code));
}

// Utility functions from the standard library.

static unsigned int strlen(const char* c_string) {
  const char* i = c_string;
  while (*i) i++;
  return i - c_string;
}

static int strcmp(const char* l, const char* r) {
  while (*l) {
    if (*l - *r) return *l - *r;
    ++l, ++r;
  }
  return *l - *r;
}

static void* memcpy(void* restrict dest, const void* restrict src,
                    unsigned int n) {
  char* o = dest;
  char* const end = o + n;
  const char* i = src;
  while (o != end) *o++ = *i++;
  return dest;
}
