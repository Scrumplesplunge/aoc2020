// Input: a comma-separated list of positive integers.
// The numbers represent the first few starting values in a game that
// proceeds in turns. In each turn, you should consider the previous value. If
// that value is appearing for the first time, the next value should be 0.
// Otherwise, the next value should be the amount of time between the last turn
// and the time when that number was last said prior to that turn.
// Part 1: Calculate the number produced in the 2020th turn.
// Part 2: Calculate the number produced in the 30000000th turn.
//
// Approach: this problem can be solved more or less naively, and can't really
// be improved much beyond that. The main insight is to use an array indexed by
// value to store the time when that value was last said. Since the starting
// values are all much smaller than 2020 and all subsequent values are bounded
// in size by the current turn index, the array index will never exceed 30000000
// in size.
//
// One interesting observation I made with this puzzle is that if I use 0 as the
// "not seen" value, the code is much slower. This is true even if I omit the
// memset to 0 which is logically redundant.

#include "util/die.h"
#include "util/memset.h"
#include "util/print_int.h"
#include "util/read_int.h"

static unsigned spoken[30000001];
int main() {
  char buffer[128];
  const int length = read(STDIN_FILENO, buffer, sizeof(buffer));
  if (length <= 0) die("read");
  if (buffer[length - 1] != '\n') die("newline");
  const char* i = buffer;
  unsigned turn = 0;
  unsigned last_number;
  memset(spoken, -1, sizeof(spoken));
  while (true) {
    ++turn;
    i = read_int(i, &last_number);
    spoken[last_number] = turn;
    if (*i == '\n') break;
    if (*i != ',') die("comma");
    i++;
  }
  while (turn < 2020) {
    const unsigned answer =
        spoken[last_number] != (unsigned)-1 ? turn - spoken[last_number] : 0;
    spoken[last_number] = turn;
    turn++;
    last_number = answer;
  }
  print_int(last_number);
  while (turn < 30000000) {
    const unsigned answer =
        spoken[last_number] != (unsigned)-1 ? turn - spoken[last_number] : 0;
    spoken[last_number] = turn;
    turn++;
    last_number = answer;
  }
  print_int(last_number);
}
