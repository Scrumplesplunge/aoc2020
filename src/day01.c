// Input: A list of distinct numbers between 0 and 2020.
// Part 1: Find the product of two different numbers that sum to 2020.
// Part 2: Find the product of three different numbers that sum to 2020.
//
// Approach: we will optimize the brute force approach by using an array of
// bools to model a set: set[x] is true if x is present in the array. We can now
// perform a linear pass for part 1 (checking set[2020 - x] for each x), and
// a quadratic search for part 2 (checking set[2020 - x - y] for each x, y).

#include "util/die.h"
#include "util/print_int.h"
#include "util/read_int.h"

static char buffer[1024];
enum { max_numbers = 200 };
// The list of input numbers.
static unsigned numbers[max_numbers];
static int n;
// A set indexed by values from the numbers array, which is 1 iff the value is
// in the list.
static bool set[2021];

// Parse the input into `numbers` and `set`.
static void read_input() {
  int len = read(STDIN_FILENO, buffer, sizeof(buffer) - 1);
  if (len <= 0) die("bad");
  if (buffer[len - 1] != '\n') die("bad");
  const char* i = buffer;
  const char* const end = buffer + len;
  while (i != end) {
    if (n == max_numbers) die("too many");
    i = read_int(i, &numbers[n]);
    if (numbers[n] > 2020) die("too large");
    set[numbers[n]] = true;
    n++;
    if (*i != '\n') die("bad");
    i++;
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
    unsigned remaining = 2020 - numbers[i];
    for (int j = i + 1; j < n; j++) {
      // Ensure that the value will be within the bounds of the set array.
      if (numbers[j] > remaining) continue;
      const unsigned num_k = remaining - numbers[j];
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
