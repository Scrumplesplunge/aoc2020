#include "util/die.h"
#include "util/printf.h"

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
  printf("%u\n", part1(width, height));
  printf("%llu\n", part2(width, height));
}
