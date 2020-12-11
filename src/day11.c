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

char input[12288];
int size, width, height;

static void read_input() {
  char temp[12288];
  const int input_size = read(STDIN_FILENO, temp, sizeof(temp));
  if (input_size <= 0) die("read");
  if (temp[input_size - 1] != '\n') die("newline");
  const char* i = temp;
  while (*i != '\n') i++;
  const int input_width = i - temp;
  const int input_stride = input_width + 1;
  if (input_size % input_stride != 0) die("shape");
  const int input_height = input_size / input_stride;
  // Move the input so that it has a border of floor.
  const int stride = input_stride + 2;
  memset(input, '.', (input_width + 3) * (input_height + 2));
  for (int y = 0; y < input_height; y++) {
    const char* source = temp + y * input_stride;
    char* destination = input + (y + 1) * stride + 1;
    memcpy(destination, source, input_width);
  }
  width = input_width + 2;
  height = input_height + 2;
  size = (width + 1) * height;
  for (int y = 0; y < height; y++) {
    input[y * stride + width] = '\n';
  }
}

char buffers[2][12288];
static int part1() {
  memcpy(buffers[0], input, size);
  memcpy(buffers[1], input, size);
  _Bool changed = 1;
  const int stride = width + 1;
  // Iterate until the state does not change.
  for (int round = 0; changed; round++) {
    changed = 0;
    const _Bool parity = round % 2;
    const char* const source = buffers[parity];
    char* const destination = buffers[1 - parity];
    for (int y = 0; y < height; y++) {
      for (int x = 0; x < width; x++) {
        const char* cell = &source[y * stride + x];
        int adjacent = 0;
        for (int dy = -1; dy <= 1; dy++) {
          for (int dx = -1; dx <= 1; dx++) {
            if (dx == 0 && dy == 0) continue;
            if (cell[stride * dy + dx] == '#') adjacent++;
          }
        }
        if (*cell == 'L' && adjacent == 0) {
          changed = 1;
          destination[y * stride + x] = '#';
        } else if (*cell == '#' && adjacent >= 4) {
          changed = 1;
          destination[y * stride + x] = 'L';
        } else {
          destination[y * stride + x] = *cell;
        }
      }
    }
  }
  int total = 0;
  for (int y = 0; y < height; y++) {
    for (int x = 0; x < width; x++) {
      if (buffers[0][y * stride + x] == '#') total++;
    }
  }
  return total;
}

char direction_buffers[8][12288];
static int part2() {
  const int stride = width + 1;
  memcpy(buffers[0], input, size);
  memcpy(buffers[1], input, size);
  for (int i = 0; i < 8; i++) {
    memset(direction_buffers[i], '.', size);
    for (int j = 0; j < size; j += stride) {
      direction_buffers[i][j + width] = '\n';
    }
  }
  _Bool changed = 1;
  // Iterate until the state does not change.
  for (int round = 0; changed; round++) {
    changed = 0;
    const _Bool parity = round % 2;
    const char* const source = buffers[parity];
    char* const destination = buffers[1 - parity];
    // Compute the direction buffers.
    const int directions[8][2] = {{-1, -1}, {0, -1}, {1, -1}, {-1, 0},
                                  {1, 0},   {-1, 1}, {0, 1},  {1, 1}};
    for (int i = 0; i < 8; i++) {
      const int dx = directions[i][0], dy = directions[i][1];
      if (i < 4) {
        for (int y = 1; y < height - 1; y++) {
          for (int x = 1; x < width - 1; x++) {
            int x2 = x + dx, y2 = y + dy;
            const char obstruction = source[y2 * stride + x2];
            const char behind = direction_buffers[i][y2 * stride + x2];
            direction_buffers[i][y * stride + x] =
                obstruction != '.' ? obstruction : behind;
          }
        }
      } else {
        for (int y = height - 2; y >= 1; y--) {
          for (int x = width - 2; x >= 1; x--) {
            int x2 = x + dx, y2 = y + dy;
            const char obstruction = source[y2 * stride + x2];
            const char behind = direction_buffers[i][y2 * stride + x2];
            direction_buffers[i][y * stride + x] =
                obstruction != '.' ? obstruction : behind;
          }
        }
      }
    }
    // Compute the adjacency for the round.
    for (int y = 0; y < height; y++) {
      for (int x = 0; x < width; x++) {
        const char* cell = &source[y * stride + x];
        int adjacent = 0;
        for (int i = 0; i < 8; i++) {
          if (direction_buffers[i][y * stride + x] == '#') adjacent++;
        }
        if (*cell == 'L' && adjacent == 0) {
          changed = 1;
          destination[y * stride + x] = '#';
        } else if (*cell == '#' && adjacent >= 5) {
          changed = 1;
          destination[y * stride + x] = 'L';
        } else {
          destination[y * stride + x] = *cell;
        }
      }
    }
  }
  int total = 0;
  for (int y = 0; y < height; y++) {
    for (int x = 0; x < width; x++) {
      if (buffers[0][y * stride + x] == '#') total++;
    }
  }
  return total;
}

int main() {
  read_input();
  print_int(part1());
  print_int(part2());
}
