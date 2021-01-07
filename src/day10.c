// Input: A list of distinct numbers which, when sorted, increase by no more
// than 3 each time. Additionally, the smallest number must be at most 3.
// Part 1: Consider the difference between each number and the next smallest
// number. Count the number of pairs which have a difference of 1 and
// a difference of 3 and return the product of these two numbers.
// Part 2: Count the number of distinct increasing sequences among the set of
// numbers which start with a number no larger than 3, do not exceed
// a difference of 3 at each step, and do include the largest number.
//
// Approach: we can start by sorting the input, since each part will benefit
// from having the numbers in increasing order.
//
// For part 1, we can perform a single linear pass over the sorted input and
// count how many times we see a difference of 1 or a difference of 3.
//
// For part 2, the naive approach would be far too slow. To do this efficiently,
// we can use a dynamic programming solution. We will compute the number of
// valid sequence prefixes starting at each adapter. To compute this value for
// a given adapter, we only need to consider the prefixes for each adapter up to
// 3 smaller than the current one, so we can compute them from smallest to
// largest in a single O(n) pass.

#include "util/die.h"
#include "util/print_int64.h"
#include "util/read_int16.h"

enum { max_numbers = 256 };
static unsigned short numbers[max_numbers];
static int num_numbers;

static void read_input() {
  char buffer[512];
  const int len = read(STDIN_FILENO, buffer, sizeof(buffer));
  if (len <= 0) die("read");
  if (buffer[len - 1] != '\n') die("newline");
  const char* i = buffer;
  const char* const end = buffer + len;
  while (i != end) {
    if (num_numbers == max_numbers) die("too many");
    i = read_int16(i, &numbers[num_numbers++]);
    if (*i != '\n') die("line");
    i++;
  }
  for (int i = 0; i < num_numbers; i++) {
    int min = i;
    for (int j = i; j < num_numbers; j++) {
      if (numbers[j] < numbers[min]) min = j;
    }
    unsigned short temp = numbers[i];
    numbers[i] = numbers[min];
    numbers[min] = temp;
  }
}

static int part1() {
  int counts[4] = {0};
  int value = 0;
  for (int i = 0; i < num_numbers; i++) {
    int delta = numbers[i] - value;
    if (delta > 3) die("bad input");
    value = numbers[i];
    counts[delta]++;
  }
  counts[3]++;
  return counts[1] * counts[3];
}

static unsigned long long arrangements_buffer[3 * max_numbers + 10];
static unsigned long long part2() {
  // arrangements[i] is the number of arrangements of adapters resulting in
  // a joltage level of i. It is offset by 2 into the buffer so that the loop
  // doesn't have to have special cases for the first few elements.
  unsigned long long* arrangements = arrangements_buffer + 2;
  arrangements[0] = 1;
  for (int i = 0; i < num_numbers; i++) {
    const int x = numbers[i];
    arrangements[x] =
        arrangements[x - 1] + arrangements[x - 2] + arrangements[x - 3];
  }
  return arrangements[numbers[num_numbers - 1]];
}

int main() {
  read_input();
  print_int64(part1());
  print_int64(part2());
}
