#pragma GCC system_header

#ifndef PRINTF_H_
#define PRINTF_H_

#include "memcpy.h"
#include "stdarg.h"

// Format an unsigned integer to the given string, returning the first character
// after the newly formatted integer.
static char* putu(char* o, unsigned x) {
  char buffer[16];
  char* const end = buffer + sizeof(buffer);
  char* i = end;
  do {
    *(--i) = '0' + (x % 10);
    x /= 10;
  } while (x);
  while (i != end) *o++ = *i++;
  return o;
}

// Print an integer in decimal, followed by a newline.
static char* putu64(char* o, unsigned long long x) {
  char buffer[24] = {0};
  // Compute the decimal format one bit at a time by doubling and carrying BCD.
  for (int i = 63; i >= 0; i--) {
    for (int j = 0; j < 24; j++) buffer[j] *= 2;
    if ((x >> i) & 1) buffer[23]++;
    char carry = 0;
    for (int j = 23; j >= 0; j--) {
      char temp = buffer[j] + carry;
      buffer[j] = temp % 10;
      carry = temp / 10;
    }
  }
  // Compute the most significant digit and truncate the output.
  int i = 0;
  while (buffer[i] == 0) i++;
  const int start = i < 23 ? i : 23;
  for (int j = start; j < 24; j++) *o++ = '0' + buffer[j];
  return o;
}

// Format a string into the given buffer, returning the number of characters
// that were written. This is a very crude subset of printf functionality.
__attribute__((format(printf, 2, 0)))
static int vsprintf(char* buffer, const char* format, va_list args) {
  char* o = buffer;
  while (1) {
    const char* string = format;
    while (*format && *format != '%') format++;
    memcpy(o, string, format - string);
    o += format - string;
    if (*format == '\0') return o - buffer;
    // Otherwise, *format is '%'.
    format++;
    const char c = *format++;
    switch (c) {
      case 'l':
        if (*format++ != 'l') break;
        if (*format++ != 'u') break;
        o = putu64(o, va_arg(args, unsigned long long));
        break;
      case 'u':
        o = putu(o, va_arg(args, unsigned));
        break;
      case 's': {
        const char* s = va_arg(args, const char*);
        const unsigned l = strlen(s);
        memcpy(o, s, l);
        o += l;
        break;
      }
      default:
        *o++ = c;
        break;
    }
  }
}

typedef struct {
  const int fd;
} FILE;

static const FILE _stdin = {.fd = STDIN_FILENO},
                  _stdout = {.fd = STDOUT_FILENO},
                  _stderr = {.fd = STDERR_FILENO};
static FILE* const stdin = &_stdin;
static FILE* const stdout = &_stdout;
static FILE* const stderr = &_stderr;

__attribute__((format(printf, 2, 0)))
static int vfprintf(FILE* f, const char* format, va_list args) {
  char buffer[4096];
  const int length = vsprintf(buffer, format, args);
  return write(f->fd, buffer, length);
}

__attribute__((format(printf, 2, 3)))
static int fprintf(FILE* f, const char* format, ...) {
  va_list args;
  va_start(args, format);
  return vfprintf(f, format, args);
}

__attribute__((format(printf, 1, 2)))
static int printf(const char* format, ...) {
  va_list args;
  va_start(args, format);
  return vfprintf(stdout, format, args);
}

#endif  // PRINTF_H_
