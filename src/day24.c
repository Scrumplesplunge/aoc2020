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

enum { max_chain = 30, grid_size = 64 };
enum direction { done, flip, e, se, sw, w, nw, ne };

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

static _Bool grid[grid_size][grid_size];

static int part1() {
  _Bool* const origin = &grid[grid_size / 2][grid_size / 2];
  const int step[] = {
    [ne] = -grid_size - 1,
    [nw] = -grid_size,
    [e] = -1,
    [w] = 1,
    [se] = grid_size,
    [sw] = grid_size + 1,
  };
  unsigned char* i = steps;
  _Bool* position = origin;
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
  for (int y = 0; y < grid_size; y++) {
    for (int x = 0; x < grid_size; x++) {
      total += grid[y][x];
    }
  }
  return total;
}

int main() {
  read_input();
  print_int(part1());
}
