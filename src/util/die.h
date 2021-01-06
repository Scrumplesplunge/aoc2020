#pragma once

#ifndef NDEBUG
#include "strlen.h"
#endif

// Exit the program with an error message.
__attribute__((noreturn)) static void die(const char* message) {
#ifdef NDEBUG
  (void)message;
#else
  write(STDERR_FILENO, message, strlen(message));
  write(STDERR_FILENO, "\n", 1);
#endif
  exit(1);
}
