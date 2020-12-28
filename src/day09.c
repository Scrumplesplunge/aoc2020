#include "util/die.h"
#include "util/printf.h"
#include "util/read_int64.h"

enum { max_numbers = 1024 };
static char buffer[16384];
static unsigned char prelude_size = 25;
static unsigned long long numbers[max_numbers];
static int num_numbers;

static void read_input() {
  const int len = read(STDIN_FILENO, buffer, sizeof(buffer));
  if (len <= 0) die("read");
  if (buffer[len - 1] != '\n') die("newline");
  const char* i = buffer;
  const char* const end = buffer + len;
  // Processing for the test case: we look for `#N` on the first line. If
  // present, we use `N` as the prelude_size instead of 25.
  if (*i == '#') {
    i++;
    unsigned long long temp;
    i = read_int64(i, &temp);
    if (*i != '\n') die("syntax");
    i++;
    if (temp > 25) die("prelude too big");
    prelude_size = temp;
  }
  while (i != end) {
    i = read_int64(i, &numbers[num_numbers++]);
    if (*i != '\n') die("line");
    i++;
  }
  if (num_numbers < prelude_size) die("too small");
}

static _Bool has_sum(unsigned long long window[25],
                     unsigned long long value) {
  for (int i = 0; i < prelude_size; i++) {
    for (int j = i + 1; j < prelude_size; j++) {
      if (value == window[i] + window[j]) return 1;
    }
  }
  return 0;
}

static unsigned long long part1() {
  unsigned long long window[25];
  // Read the prelude.
  for (int i = 0; i < prelude_size; i++) window[i] = numbers[i];
  // Search for the value.
  for (int i = prelude_size; i < num_numbers; i++) {
    if (!has_sum(window, numbers[i])) {
      return numbers[i];
    }
    window[i % prelude_size] = numbers[i];
  }
  die("not found");
}

static unsigned long long part2(unsigned long long key) {
  unsigned long long sum = 0;
  int first = 0, last = 0;
  while (1) {
    if (sum == key) break;
    if (sum < key) {
      if (last == num_numbers) die("not found");
      sum += numbers[last++];
    } else {
      sum -= numbers[first++];
    }
  }
  unsigned long long min = -1, max = 0;
  for (int i = first; i < last; i++) {
    if (numbers[i] < min) min = numbers[i];
    if (numbers[i] > max) max = numbers[i];
  }
  return min + max;
}

int main() {
  read_input();
  const unsigned long long key = part1();
  printf("%llu\n%llu\n", key, part2(key));
}
