#include "util/die.h"
#include "util/print_int64.h"

// GCC automatically generates calls to __udivdi3 and __umoddi3 when attempting
// division with `unsigned long long` variables, so we need to provide
// a definition. However, it seems that these magic symbols don't get GC'd
// properly when unused, so uncommenting this code causes all solvers to get
// bigger.

struct divmod_result {
  unsigned long long quotient;
  unsigned long long remainder;
};

static __attribute__((pure)) struct divmod_result divmod(unsigned long long a,
                                                         unsigned long long b) {
  unsigned long long accumulator = 0;
  unsigned long long value = b;
  unsigned long long factor = 1;
  while ((value << 1) <= a) {
    value <<= 1;
    factor <<= 1;
  }
  while (factor) {
    if (value <= a) {
      a -= value;
      accumulator += factor;
    }
    value >>= 1;
    factor >>= 1;
  }
  return (struct divmod_result){.quotient = accumulator, .remainder = a};
}

__attribute__((pure)) unsigned long long __udivdi3(unsigned long long a,
                                                   unsigned long long b) {
  return divmod(a, b).quotient;
}

__attribute__((pure)) unsigned long long __umoddi3(unsigned long long a,
                                                   unsigned long long b) {
  return divmod(a, b).remainder;
}

static _Bool is_digit(char c) {
  return '0' <= c && c <= '9';
}

// Read a decimal integer from the string at input into value, returning the
// address of the first byte after the integer.
static const char* read_int(const char* input, unsigned* value) {
  if (!is_digit(*input)) die("int");
  unsigned temp = 0;
  while (is_digit(*input)) {
    temp = 10 * temp + (*input - '0');
    input++;
  }
  *value = temp;
  return input;
}

// Round x up to the next multiple of k.
static unsigned round_up(unsigned x, unsigned k) {
  return (x + k - 1) / k * k;
}

static int part1(const char* input) {
  unsigned earliest_departure;
  enum { max_buses = 16 };
  unsigned buses[max_buses];
  int num_buses = 0;
  input = read_int(input, &earliest_departure);
  if (*input != '\n') die("bad input");
  do {
    input++;
    if (*input == 'x') {
      input++;
    } else {
      if (num_buses == max_buses) die("too large");
      input = read_int(input, &buses[num_buses++]);
    }
  } while (*input == ',');
  if (*input != '\n') die("bad input");
  unsigned bus = 0, time = round_up(earliest_departure, buses[0]);
  for (int i = 1; i < num_buses; i++) {
    const unsigned new_time = round_up(earliest_departure, buses[i]);
    if (new_time < time) {
      bus = i;
      time = new_time;
    }
  }
  return buses[bus] * (time - earliest_departure);
}

static unsigned long long gcd(unsigned long long a, unsigned long long b) {
  while (b != 0) {
    const unsigned long long temp = b;
    b = a % b;
    a = temp;
  }
  return a;
}

static unsigned long long lcm(unsigned long long a, unsigned long long b) {
  return a / gcd(a, b) * b;
}

static unsigned long long part2(const char* input) {
  unsigned earliest_departure;
  enum { max_buses = 128 };
  unsigned buses[max_buses];
  int num_buses = 0;
  input = read_int(input, &earliest_departure);
  if (*input != '\n') die("bad input");
  do {
    input++;
    if (num_buses == max_buses) die("too large");
    if (*input == 'x') {
      input++;
      buses[num_buses++] = 0;
    } else {
      input = read_int(input, &buses[num_buses++]);
    }
  } while (*input == ',');
  if (*input != '\n') die("bad input");
  unsigned long long earliest = 0;
  unsigned long long period = 1;
  for (int i = 0; i < num_buses; i++) {
    if (buses[i] == 0) continue;
    // Invariant: `earliest` is the earliest time when the first i buses arrive
    // at the right minute. `period` is the amount of time until that happens
    // again.
    while ((earliest + i) % buses[i] != 0) earliest += period;
    period = lcm(period, buses[i]);
  }
  return earliest;
}

static char buffer[256];
int main() {
  int length = read(STDIN_FILENO, buffer, sizeof(buffer) - 1);
  if (length <= 0) die("read");
  if (buffer[length - 1] != '\n') die("newline");
  buffer[length] = '\0';
  print_int64(part1(buffer));
  print_int64(part2(buffer));
}
