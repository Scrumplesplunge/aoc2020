// Input: A list of boarding passes consisting of 10 uppercase letters. The
// first 7 identify the row ('B' for back half, 'F' for front half), while the
// last 3 identify the column ('L' for left half, 'R' for right half). Each
// letter recursively denotes which half the seat is in.
// Part 1: Rows are numbered 0-127, columns are numbered 0-7. The ID of a seat
// is its row * 8 plus its column. Find the highest seat ID on a boarding pass.
// Part 2: Some rows at the front and back of the plane have no seats, and the
// front-most and back-most rows may have missing seats. You know that you are
// not sat in the front or back rows, and that all seats in the flight are
// booked. Your seat is the only seat that is not taken by any ticket. Find your
// seat ID.
//
// Approach: First, we can observe that if we replace 'B' and 'R' with '1', and
// replace 'F' and 'L' with '0', then a boarding pass spells out its own seat ID
// in binary. Thus, we can trivially parse a boarding pass into a seat ID. While
// parsing this, we will discover the maximum seat ID. For part 2, the only
// thing we are interested in is the existence of each ID, so we can represent
// every possible seat with a 128-byte bitset (where a byte is a row of the
// plane), look for a byte which is not 0xFF to find our row, and deduce our
// column from the missing bit.

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
