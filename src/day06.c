#include "util/die.h"
#include "util/popcount.h"
#include "util/is_lower.h"
#include "util/printf.h"

static char input[32768];

int main() {
  const int len = read(STDIN_FILENO, input, sizeof(input));
  if (len <= 0) die("read");
  if (input[len - 1] != '\n') die("newline");
  char* i = input;
  char* const end = input + len;
  int any_count = 0, all_count = 0;
  while (i < end) {
    // Mask of answers seen in the group. Bit i is set if ('a' + i) is present.
    unsigned any = 0, all = 0x03FFFFFF;
    // Iterate over a group of passengers.
    while (i < end) {
      if (*i == '\n') {
        i++;
        break;
      }
      // Iterate over a single passenger's answers.
      unsigned mask = 0;
      while (*i != '\n') {
        if (!is_lower(*i)) die("bad");
        mask |= 1 << (*i - 'a');
        i++;
      }
      any |= mask;
      all &= mask;
      i++;
    }
    any_count += popcount(any);
    all_count += popcount(all);
  }
  printf("%u\n%u\n", any_count, all_count);
}
