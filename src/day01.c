#include "util/die.h"
#include "util/print_int.h"

static _Bool is_digit(char c) {
  return '0' <= c && c <= '9';
}

static _Bool is_whitespace(char c) {
  return c == ' ' || c == '\n';
}

// Advance the given pointer until it does not point at whitespace.
static const char* skip_whitespace(const char* input) {
  while (is_whitespace(*input)) input++;
  return input;
}

// Read a decimal integer from the string at input into value, returning the
// address of the first byte after the integer.
static const char* read_int(const char* input, int* value) {
  if (!is_digit(*input)) return NULL;
  int temp = 0;
  while (is_digit(*input)) {
    temp = 10 * temp + (*input - '0');
    input++;
  }
  *value = temp;
  return input;
}

static char buffer[1024];
enum { max_numbers = 200 };
// The list of input numbers.
static int numbers[max_numbers];
static int n;
// A set indexed by values from the numbers array, which is 1 iff the value is
// in the list.
static char set[2021];

// Parse the input into `numbers` and `set`.
static void read_input() {
  int len = read(STDIN_FILENO, buffer, sizeof(buffer) - 1);
  if (len == -1) die("read");
  const char* i = buffer;
  const char* const end = buffer + len;
  while (i != end) {
    i = skip_whitespace(i);
    if (i == end) break;
    if (n == max_numbers) die("too many");
    i = read_int(skip_whitespace(i), &numbers[n]);
    if (i == NULL) die("bad input");
    if (numbers[n] > 2020) die("too large");
    set[numbers[n]] = 1;
    n++;
  }
}

static int part1(void) {
  for (int i = 0; i < n; i++) {
    if (set[2020 - numbers[i]]) {
      return numbers[i] * (2020 - numbers[i]);
    }
  }
  die("not found");
}

static int part2(void) {
  for (int i = 0; i < n; i++) {
    int remaining = 2020 - numbers[i];
    for (int j = i + 1; j < n; j++) {
      if (numbers[j] > remaining) continue;
      const int num_k = remaining - numbers[j];
      if (set[num_k]) {
        return numbers[i] * numbers[j] * num_k;
      }
    }
  }
  die("not found");
}

int main() {
  read_input();
  print_int(part1());
  print_int(part2());
}
