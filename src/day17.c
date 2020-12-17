// Exit the program with a given error message.
__attribute__((noreturn)) static void die(const char* message) {
  write(STDERR_FILENO, message, strlen(message));
  write(STDERR_FILENO, "\n", 1);
  exit(1);
}

// Print an integer in decimal, followed by a newline.
static void print_int(int x) {
  const _Bool negative = x < 0;
  if (!negative) x = -x;
  char buffer[16];
  buffer[15] = '\n';
  int i = 15;
  do {
    --i;
    buffer[i] = '0' - (x % 10);
    x /= 10;
  } while (x);
  if (negative) buffer[--i] = '-';
  write(STDOUT_FILENO, buffer + i, 16 - i);
}

enum { size_x = 32, size_y = 32, size_z = 16, size_w = 16 };
_Bool part1_cells[2][size_z][size_y][size_x];
_Bool part2_cells[2][size_w][size_z][size_y][size_x];

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
    _Bool (*input)[size_z][size_y][size_x] = part2_cells[parity];
    _Bool (*output)[size_z][size_y][size_x] = part2_cells[1 - parity];
    for (int w = 1; w < size_w - 1; w++) {
      for (int z = 1; z < size_z - 1; z++) {
        for (int y = 1; y < size_y - 1; y++) {
          for (int x = 1; x < size_x - 1; x++) {
            const _Bool populated = input[w][z][y][x];
            int neighbours = -populated;
            for (int dw = -1; dw <= 1; dw++) {
              for (int dz = -1; dz <= 1; dz++) {
                for (int dy = -1; dy <= 1; dy++) {
                  for (int dx = -1; dx <= 1; dx++) {
                    if (input[w + dw][z + dz][y + dy][x + dx]) neighbours++;
                  }
                }
              }
            }
            if (populated) {
              output[w][z][y][x] = neighbours == 2 || neighbours == 3;
            } else {
              output[w][z][y][x] = neighbours == 3;
            }
          }
        }
      }
    }
  }
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
