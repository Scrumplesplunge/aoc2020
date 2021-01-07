// Input: a list of groups, separated by blank lines. In each group, there is
// one line per person, and the line will consist of one or more lowercase
// letters indicating which questions that person answered yes to.
// Part 1: For each group, find the number of questions which have a yes answer
// from at least one person. Produce the sum of these counts across all groups.
// Part 2: For each group, find the number of questions which have a yes answer
// from *every* person. Produce the sum of these counts across all groups.
//
// Approach: this can be solved easily with bitsets again. We can use a 26-bit
// number to express the set of questions which received a yes answer from
// a person. For part 1, we can take the bitwise or across the group, and for
// part 2, we can take the bitwise and. Finally, we can count up the bits that
// are set to 1 in each group and accumulate the output.

#include "util/die.h"
#include "util/popcount.h"
#include "util/print_int.h"
#include "util/is_lower.h"

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
  print_int(any_count);
  print_int(all_count);
}
