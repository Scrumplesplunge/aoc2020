#include "util/die.h"
#include "util/memset.h"
#include "util/print_int.h"

enum { grid_size = 256, max_chain = grid_size / 2 - 101 };
enum direction { e, se, sw, w, nw, ne, done, flip };

unsigned char steps[32768];

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
    const _Bool parity = day % 2;
    unsigned char *input = grid[parity];
    unsigned char *output = grid[1 - parity];
    memset(output, 0, sizeof(grid[parity]));
    for (int i = 0; i < 6; i++) {
      for (int j = begin; j < end; j++) output[j] += input[j + step[i]];
    }
    for (int i = begin; i < end; i++) {
      const _Bool active = input[i];
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
