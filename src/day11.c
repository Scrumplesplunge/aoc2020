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

enum { max_size = 128 };

// Grid dimensions. All instances of `grid` will use this size.
int grid_width, grid_height;

enum cell { floor, seat, person };
struct grid {
  char cells[max_size][max_size];
};

struct grid input;

static void read_input() {
  char temp[65536];
  const int length = read(STDIN_FILENO, temp, sizeof(temp));
  if (length <= 0) die("read");
  if (temp[length - 1] != '\n') die("newline");
  const char* i = temp;
  while (*i != '\n') i++;
  const int width = i - temp;
  const int stride = width + 1;
  if (length % stride != 0) die("shape");
  const int height = length / stride;
  // Move the input into the grid. Add a border of floor around the edges to
  // simplify the logic for handling them.
  for (int y = 0; y < height; y++) {
    for (int x = 0; x < width; x++) {
      char* out = &input.cells[y + 1][x + 1];
      switch (temp[y * stride + x]) {
        case 'L':
          *out = seat;
          break;
        case '#':
          *out = person;
          break;
        case '.':
          *out = floor;
          break;
        default:
          die("bad input");
      }
    }
  }
  grid_width = width + 2;
  grid_height = height + 2;
}

// Find the number of people seated once the arrangement stabilises, given
// a comfort threshold (the number of adjacent people which are needed for
// someone to vacate their seat) and a function which counts adjacent people.
// The `prepare` parameter is invoked before each stage to allow for stateful
// adjacency functions.
struct grid buffers[2];
static int find_seated(int comfort_threshold,
                       void (*prepare)(const struct grid*),
                       int (*adjacent)(const struct grid*, int, int)) {
  memcpy(&buffers[0], &input, sizeof(input));
  _Bool changed = 1;
  // Iterate until the state does not change.
  for (int round = 0; changed; round++) {
    changed = 0;
    const _Bool parity = round % 2;
    const struct grid* source = &buffers[parity];
    struct grid* const destination = &buffers[1 - parity];
    prepare(source);
    for (int y = 1; y < grid_height - 1; y++) {
      for (int x = 1; x < grid_width - 1; x++) {
        const int a = adjacent(source, x, y);
        const char cell = source->cells[y][x];
        if (cell == seat && a == 0) {
          changed = 1;
          destination->cells[y][x] = person;
        } else if (cell == person && a >= comfort_threshold) {
          changed = 1;
          destination->cells[y][x] = seat;
        } else {
          destination->cells[y][x] = source->cells[y][x];
        }
      }
    }
  }
  int total = 0;
  for (int y = 1; y < grid_height - 1; y++) {
    for (int x = 1; x < grid_width - 1; x++) {
      if (buffers[0].cells[y][x] == person) total++;
    }
  }
  return total;
}

static void noop() {}

static int part1_adjacent(const struct grid* source, int x, int y) {
  int adjacent = 0;
  for (int dy = -1; dy <= 1; dy++) {
    for (int dx = -1; dx <= 1; dx++) {
      if (dx == 0 && dy == 0) continue;
      if (source->cells[y + dy][x + dx] == person) adjacent++;
    }
  }
  return adjacent;
}

// occupied[y][x][i] is true if someone at (x, y) can see an occupied seat in
// direction i. If there is an unoccupied seat in the way, or if there is no
// seat in that direction, it will be false.
_Bool occupied[max_size][max_size][8];

static void part2_prepare(const struct grid* source) {
  // Update the occupied array.
  const int directions[8][2] = {{-1, -1}, {0, -1}, {1, -1}, {-1, 0},
                                {1, 0},   {-1, 1}, {0, 1},  {1, 1}};
  for (int i = 0; i < 4; i++) {
    const int dx = directions[i][0], dy = directions[i][1];
    for (int y = 1; y < grid_height - 1; y++) {
      for (int x = 1; x < grid_width - 1; x++) {
        int x2 = x + dx, y2 = y + dy;
        const char obstruction = source->cells[y2][x2];
        const char behind = occupied[y2][x2][i];
        occupied[y][x][i] =
            obstruction != floor ? obstruction == person : behind;
      }
    }
  }
  for (int i = 4; i < 8; i++) {
    const int dx = directions[i][0], dy = directions[i][1];
    for (int y = grid_height - 2; y >= 1; y--) {
      for (int x = grid_width - 2; x >= 1; x--) {
        int x2 = x + dx, y2 = y + dy;
        const char obstruction = source->cells[y2][x2];
        const char behind = occupied[y2][x2][i];
        occupied[y][x][i] =
            obstruction != floor ? obstruction == person : behind;
      }
    }
  }
}

static int part2_adjacent(const struct grid* source, int x, int y) {
  (void)source;
  int adjacent = 0;
  for (int i = 0; i < 8; i++) {
    adjacent += occupied[y][x][i];
  }
  return adjacent;
}

int main() {
  read_input();
  print_int(find_seated(4, noop, part1_adjacent));
  print_int(find_seated(5, part2_prepare, part2_adjacent));
}
