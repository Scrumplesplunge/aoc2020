// Input: several arithmetic expressions that must be evaluated according to
// modified rules.
// Part 1: Operator precedence is left-to-right, so 1 + 2 * 3 is (1 + 2) * 3.
// Find the sum of the results of all expressions in the input.
// Part 2: Addition binds more tightly than multiplication instead of the other
// way around, so 1 * 2 + 3 is 1 * (2 + 3). Find the sum of the results of all
// expressions in the input.
//
// Approach: this is a simple parsing problem. We will use a handwritten top
// down parser that evaluates the expression as it goes.

#include "util/die.h"
#include "util/print_int64.h"
#include "util/read_int64.h"

static const char* part1_expr(const char* i, unsigned long long* result);

static const char* part1_term(const char* i, unsigned long long* result) {
  if (*i == '(') {
    i = part1_expr(i + 1, result);
    if (*i != ')') die("brackets");
    return i + 1;
  } else {
    return read_int64(i, result);
  }
}

static const char* part1_expr(const char* i, unsigned long long* result) {
  while (*i == ' ') i++;
  i = part1_term(i, result);
  while (*i != ')' && *i != '\n') {
    while (*i == ' ') i++;
    const char op = *i++;
    while (*i == ' ') i++;
    unsigned long long temp;
    i = part1_term(i, &temp);
    switch (op) {
      case '+':
        *result += temp;
        break;
      case '*':
        *result *= temp;
        break;
      default:
        die("syntax");
    }
  }
  return i;
}

static unsigned long long part1(const char* i, const char* const end) {
  unsigned long long total = 0;
  while (i != end) {
    unsigned long long value;
    i = part1_expr(i, &value);
    total += value;
    if (*i != '\n') die("syntax");
    i++;
  }
  return total;
}

static const char* part2_expr(const char* i, unsigned long long* result);

static const char* part2_term(const char* i, unsigned long long* result) {
  if (*i == '(') {
    i = part2_expr(i + 1, result);
    if (*i != ')') die("brackets");
    return i + 1;
  } else {
    return read_int64(i, result);
  }
}

static const char* part2_sum(const char* i, unsigned long long* result) {
  while (*i == ' ') i++;
  i = part2_term(i, result);
  while (*i != ')' && *i != '\n') {
    while (*i == ' ') i++;
    if (*i != '+') break;
    i++;
    while (*i == ' ') i++;
    unsigned long long temp;
    i = part2_term(i, &temp);
    *result += temp;
  }
  return i;
}

static const char* part2_expr(const char* i, unsigned long long* result) {
  while (*i == ' ') i++;
  i = part2_sum(i, result);
  while (*i != ')' && *i != '\n') {
    while (*i == ' ') i++;
    if (*i != '*') break;
    i++;
    while (*i == ' ') i++;
    unsigned long long temp;
    i = part2_sum(i, &temp);
    *result *= temp;
  }
  return i;
}

static unsigned long long part2(const char* i, const char* const end) {
  unsigned long long total = 0;
  while (i != end) {
    unsigned long long value;
    i = part2_expr(i, &value);
    total += value;
    if (*i != '\n') die("syntax");
    i++;
  }
  return total;
}

int main() {
  char buffer[32768];
  const int length = read(STDIN_FILENO, buffer, sizeof(buffer));
  if (length <= 0) die("read");
  if (buffer[length - 1] != '\n') die("newline");
  const char* i = buffer;
  const char* const end = buffer + length;
  print_int64(part1(i, end));
  print_int64(part2(i, end));
}
