// Input: A rectangular map depicting a pattern of open space (.) and trees (#)
// that repeats horizontally.
// Part 1: Travelling with a step of (3, 1), count the number of trees that
// would be encountered before reaching the bottom edge of the map.
// Part 2: For each of (1, 1), (3, 1), (5, 1), (7, 1), (1, 2), repeat the
// process and find the product of the tree counts from each one.
//
// Approach: we can treat the X axis as (mod width) and solve the puzzle without
// copying the pattern out multiple times. The processing requires a single pass
// for each gradient and runs in time proportional to the height of the grid.

#include "util/die.h"
#include "util/print_int64.h"

static char buffer[65536];

static unsigned int solve(int width, int height, int dx, int dy) {
  unsigned int count = 0;
  int x = 0;
  const int row_stride = dy * (width + 1);
  const int end = (width + 1) * height;
  for (int row_start = row_stride; row_start < end; row_start += row_stride) {
    x += dx;
    if (x >= width) x -= width;
    if (buffer[row_start + x] == '#') count++;
  }
  return count;
}

static unsigned int part1(int width, int height) {
  return solve(width, height, 3, 1);
}

static const int cases[][2] = {{1, 1}, {3, 1}, {5, 1}, {7, 1}, {1, 2}};
enum { num_cases = sizeof(cases) / sizeof(cases[0]) };
static unsigned long long part2(int width, int height) {
  unsigned long long total = 1;
  for (int i = 0; i < num_cases; i++) {
    total *= solve(width, height, cases[i][0], cases[i][1]);
  }
  return total;
}

int main() {
  // Read the input.
  int len = read(STDIN_FILENO, buffer, sizeof(buffer) - 1);
  if (len < 0) die("read");
  int width = 0;
  while (buffer[width] != '\n') width++;
  if (len % (width + 1)) die("bad input");
  const int height = len / (width + 1);
  print_int64(part1(width, height));
  print_int64(part2(width, height));
}
