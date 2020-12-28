#include "util/die.h"

// Print an integer in decimal, followed by a newline.
static void print_int64(unsigned long long x) {
  char buffer[24] = {[23] = '\n'};
  // Compute the decimal format one bit at a time by doubling and carrying BCD.
  for (int i = 63; i >= 0; i--) {
    for (int j = 0; j < 23; j++) buffer[j] *= 2;
    if ((x >> i) & 1) buffer[22]++;
    char carry = 0;
    for (int j = 22; j >= 0; j--) {
      char temp = buffer[j] + carry;
      buffer[j] = temp % 10;
      carry = temp / 10;
    }
  }
  // Compute the most significant digit and truncate the output.
  int i = 0;
  while (buffer[i] == 0) i++;
  const int start = i < 22 ? i : 22;
  for (int j = start; j < 23; j++) buffer[j] += '0';
  write(STDOUT_FILENO, buffer + start, 24 - start);
}

static _Bool is_digit(char c) {
  return '0' <= c && c <= '9';
}

// Read a decimal integer from the string at input into value, returning the
// address of the first byte after the integer.
static const char* read_int(const char* input, unsigned long long* value) {
  if (!is_digit(*input)) die("int");
  unsigned long long temp = 0;
  while (is_digit(*input)) {
    temp = 10 * temp + (*input - '0');
    input++;
  }
  *value = temp;
  return input;
}

static const char* part1_expr(const char* i, unsigned long long* result);

static const char* part1_term(const char* i, unsigned long long* result) {
  if (*i == '(') {
    i = part1_expr(i + 1, result);
    if (*i != ')') die("brackets");
    return i + 1;
  } else {
    return read_int(i, result);
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
    return read_int(i, result);
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
