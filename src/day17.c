#include "util/die.h"
#include "util/memset.h"
#include "util/print_int.h"

enum { size_x = 32, size_y = 32, size_z = 16, size_w = 16 };
static _Bool part1_cells[2][size_z][size_y][size_x];
static unsigned char part2_cells[2][size_w][size_z][size_y][size_x];

static void read_input() {
  char buffer[256];
  const int length = read(STDIN_FILENO, buffer, sizeof(buffer));
  if (length <= 0) die("read");
  if (buffer[length - 1] != '\n') die("newline");
  buffer[length] = '\0';
  char* i = buffer;
  while (*i != '\n') i++;
  const int width = i - buffer;
  if (length % (width + 1) != 0) die("shape");
  const int height = length / (width + 1);
  if (width > 10 || height > 10) die("not enough margin");
  const int offset_x = (size_x - width) / 2;
  const int offset_y = (size_y - height) / 2;
  const int offset_z = size_z / 2;
  const int offset_w = size_w / 2;
  for (int y = 0; y < height; y++) {
    for (int x = 0; x < width; x++) {
      const char c = buffer[y * (width + 1) + x];
      if (c != '#' && c != '.') die("bad");
      part1_cells[0][offset_z][offset_y + y][offset_x + x] = c == '#';
      part2_cells[0][offset_w][offset_z][offset_y + y][offset_x + x] = c == '#';
    }
  }
}

static int part1() {
  for (int i = 0; i < 6; i++) {
    const _Bool parity = i % 2;
    _Bool (*input)[size_y][size_x] = part1_cells[parity];
    _Bool (*output)[size_y][size_x] = part1_cells[1 - parity];
    for (int z = 1; z < size_z - 1; z++) {
      for (int y = 1; y < size_y - 1; y++) {
        for (int x = 1; x < size_x - 1; x++) {
          const _Bool populated = input[z][y][x];
          int neighbours = -populated;
          for (int dz = -1; dz <= 1; dz++) {
            for (int dy = -1; dy <= 1; dy++) {
              for (int dx = -1; dx <= 1; dx++) {
                if (input[z + dz][y + dy][x + dx]) neighbours++;
              }
            }
          }
          if (populated) {
            output[z][y][x] = neighbours == 2 || neighbours == 3;
          } else {
            output[z][y][x] = neighbours == 3;
          }
        }
      }
    }
  }
  int populated = 0;
  for (int z = 0; z < size_z; z++) {
    for (int y = 0; y < size_y; y++) {
      for (int x = 0; x < size_x; x++) {
        populated += part1_cells[0][z][y][x];
      }
    }
  }
  return populated;
}

static int part2() {
  for (int i = 0; i < 6; i++) {
    const _Bool parity = i % 2;
    unsigned char (*input)[size_z][size_y][size_x] = part2_cells[parity];
    unsigned char (*output)[size_z][size_y][size_x] = part2_cells[1 - parity];
    // Accumulate the neighbours for each cell. Instead of doing it one cell at
    // a time, we do every cell at once, accumulating the partial totals in the
    // output cells before replacing those neighbour counts with cell values
    // later. This has much better cache locality as the innermost loops only
    // access two locations that always move forwards linearly.
    memset(output, 0, sizeof(part2_cells[1 - parity]));
    for (int dw = -1; dw <= 1; dw++) {
      for (int dz = -1; dz <= 1; dz++) {
        for (int dy = -1; dy <= 1; dy++) {
          for (int dx = -1; dx <= 1; dx++) {
            if (dx == 0 && dy == 0 && dz == 0 && dw == 0) continue;
            for (int w = size_w / 2; w < size_w - 1; w++) {
              for (int z = size_z / 2; z < size_z - 1; z++) {
                for (int y = 1; y < size_y - 1; y++) {
                  for (int x = 1; x < size_x - 1; x++) {
                    output[w][z][y][x] += input[w + dw][z + dz][y + dy][x + dx];
                  }
                }
              }
            }
          }
        }
      }
    }
    // Having accumulated the population counts, use them to establish the new
    // cell values.
    for (int w = size_w / 2; w < size_w - 1; w++) {
      for (int z = size_z / 2; z < size_z - 1; z++) {
        for (int y = 1; y < size_y - 1; y++) {
          for (int x = 1; x < size_x - 1; x++) {
            const _Bool populated = input[w][z][y][x];
            const unsigned char neighbours = output[w][z][y][x];
            if (populated) {
              output[w][z][y][x] = neighbours == 2 || neighbours == 3;
            } else {
              output[w][z][y][x] = neighbours == 3;
            }
            // Exploit the symmetry.
            output[size_w - w][z][y][x] = output[w][z][y][x];
            output[size_w - w][size_z - z][y][x] = output[w][z][y][x];
            output[w][size_z - z][y][x] = output[w][z][y][x];
          }
        }
      }
    }
  }
  // Count the populated cells.
  int populated = 0;
  for (int w = 0; w < size_w; w++) {
    for (int z = 0; z < size_z; z++) {
      for (int y = 0; y < size_y; y++) {
        for (int x = 0; x < size_x; x++) {
          populated += part2_cells[0][w][z][y][x];
        }
      }
    }
  }
  return populated;
}

int main() {
  read_input();
  print_int(part1());
  print_int(part2());
}
