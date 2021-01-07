// Input: sequences of directions, one sequence per line. Each sequence consists
// of {e, se, sw, w, ne, nw}, with no delimiters, representing each of the six
// directions which are adjacent to any hexagonal tile. Each tile has a black
// side and a white side, and all tiles initially have the white side face up.
// Part 1: Follow each sequence of directions and flip over the tile at the
// final position. Find the total number of tiles which are left with the black
// side face up.
// Part 2: Every day, tiles are updated according to the following rules:
//   * Unless a black tile has exactly one adjacent black tile, it is flipped to
//     white.
//   * If a white tile has exactly two adjacent black tiles, it is flipped to
//     black.
// Find the number of tiles with the black side facing upwards after 100 days.
//
// Approach: the hexagonal grid can be mapped to a square grid to make it easier
// to represent in a multidimensional array:
//      __             +--+--+      +--+--+
//   __/ W\__  skew    | W|SW| turn |NE|NW|
//  /NW\__/SW\      +--+--+--+      +--+--+--+
//  \__/  \__/  ->  |NW|  |SE|  ->  | E|  | W|
//  /NE\__/SE\      +--+--+--+      +--+--+--+
//  \__/ E\__/      |NE| E|            |SE|SW|
//     \__/         +--+--+            +--+--+
// With that in mind, it is trivial to solve both part 1 and part 2 by working
// on the square grid. By restricting the maximum length of an input sequence,
// we can use a grid which is big enough that we will never hit the edges and
// can focus only on the easy cases.

#include "util/die.h"
#include "util/memset.h"
#include "util/print_int.h"

enum { grid_size = 256, max_chain = grid_size / 2 - 101 };
enum direction { e, se, sw, w, nw, ne, done, flip };

static unsigned char steps[32768];

static void read_input() {
  char buffer[32768];
  const int length = read(STDIN_FILENO, buffer, sizeof(buffer));
  if (length <= 0) die("read");
  if (buffer[length - 1] != '\n') die("newline");
  char* i = buffer;
  char* const end = buffer + length;
  unsigned char* o = steps;
  while (i != end) {
    int chain = 0;
    while (*i != '\n') {
      if (chain == max_chain) die("too long");
      chain++;
      switch (*i++) {
        case 'e':
          *o++ = e;
          break;
        case 'w':
          *o++ = w;
          break;
        case 'n':
          switch (*i++) {
            case 'e':
              *o++ = ne;
              break;
            case 'w':
              *o++ = nw;
              break;
            default:
              die("syntax");
          }
          break;
        case 's':
          switch (*i++) {
            case 'e':
              *o++ = se;
              break;
            case 'w':
              *o++ = sw;
              break;
            default:
              die("syntax");
          }
          break;
      }
    }
    *o++ = flip;
    i++;
  }
  *o++ = done;
}

//      __             +--+--+      +--+--+
//   __/ W\__  skew    | W|SW| turn |NE|NW|
//  /NW\__/SW\      +--+--+--+      +--+--+--+
//  \__/  \__/  ->  |NW|  |SE|  ->  | E|  | W|
//  /NE\__/SE\      +--+--+--+      +--+--+--+
//  \__/ E\__/      |NE| E|            |SE|SW|
//     \__/         +--+--+            +--+--+

static unsigned char grid[2][grid_size * grid_size];

static int part1() {
  unsigned char* const origin = &grid[0][grid_size * grid_size / 2];
  const int step[] = {
    [ne] = -grid_size - 1,
    [nw] = -grid_size,
    [e] = -1,
    [w] = 1,
    [se] = grid_size,
    [sw] = grid_size + 1,
  };
  unsigned char* i = steps;
  unsigned char* position = origin;
  while (*i != done) {
    if (*i == flip) {
      *position = !*position;
      position = origin;
    } else {
      position += step[*i];
    }
    i++;
  }
  int total = 0;
  for (int i = 0; i < grid_size * grid_size; i++) total += grid[0][i];
  return total;
}

static int part2() {
  const int step[] = {
    [ne] = -grid_size - 1,
    [nw] = -grid_size,
    [e] = -1,
    [w] = 1,
    [se] = grid_size,
    [sw] = grid_size + 1,
  };
  const int begin = grid_size + 1;
  const int end = grid_size * grid_size - grid_size - 1;
  for (int day = 0; day < 100; day++) {
    const bool parity = day % 2;
    unsigned char *input = grid[parity];
    unsigned char *output = grid[1 - parity];
    memset(output, 0, sizeof(grid[parity]));
    for (int i = 0; i < 6; i++) {
      for (int j = begin; j < end; j++) output[j] += input[j + step[i]];
    }
    for (int i = begin; i < end; i++) {
      const bool active = input[i];
      const unsigned char neighbours = output[i];
      if (active) {
        output[i] = neighbours == 1 || neighbours == 2;
      } else {
        output[i] = neighbours == 2;
      }
    }
  }
  int total = 0;
  for (int i = 0; i < grid_size * grid_size; i++) total += grid[0][i];
  return total;
}

int main() {
  read_input();
  print_int(part1());
  print_int(part2());
}
