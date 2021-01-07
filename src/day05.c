#include "util/die.h"
#include "util/print_int.h"

// Bitmask for seats. seats[row] is a full row with bit i set if column i is
// populated by some boarding pass.
static unsigned char seats[128] = {0};

int main() {
  int max_id = 0;
  while (true) {
    char code[11];
    const int len = read(STDIN_FILENO, code, 11);
    if (len == 0) break;
    if (len != 11) die("read");
    if (code[10] != '\n') die("newline");
    int row = 0, column = 0;
    for (int i = 0; i < 7; i++) row = (row << 1) | (code[i] == 'B');
    for (int i = 7; i < 10; i++) column = (column << 1) | (code[i] == 'R');
    const int id = row << 3 | column;
    if (id > max_id) max_id = id;
    seats[row] |= 1 << column;
  }
  // Part 1: print the maximum boarding pass ID.
  print_int(max_id);
  // Part 2: find the id of the unpopulated seat. Some seats at the front and
  // back do not exist, so we need to disregard those.
  int start = 0;
  while (seats[start] != 255) start++;
  int end = 127;
  while (seats[end] != 255) end--;
  for (int row = start; row < end; row++) {
    if (seats[row] == 255) continue;
    for (int column = 0; column < 8; column++) {
      if (~seats[row] & (1 << column)) {
        print_int(row << 3 | column);
        return 0;
      }
    }
  }
  die("not found");
}
