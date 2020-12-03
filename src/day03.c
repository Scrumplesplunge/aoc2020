enum { stdin, stdout, stderr };
#define NULL ((void*)0)
extern int write(int fd, const void* buffer, int size);
extern int read(int fd, void* buffer, int size);
extern void exit(int code);

static unsigned int strlen(const char* c_string) {
  const char* i = c_string;
  while (*i) i++;
  return i - c_string;
}

// Exit the program with a given error message.
__attribute__((noreturn)) static void die(const char* message) {
  write(stderr, message, strlen(message));
  write(stderr, "\n", 1);
  exit(1);
}

//static _Bool is_digit(char c) {
//  return '0' <= c && c <= '9';
//}

// Print an integer in decimal, followed by a newline.
static void print_int(unsigned int x) {
  char buffer[16];
  buffer[15] = '\n';
  int i = 15;
  do {
    --i;
    buffer[i] = '0' + (x % 10);
    x /= 10;
  } while (x);
  write(stdout, buffer + i, 16 - i);
}

// Read a decimal integer from the string at input into value, returning the
// address of the first byte after the integer.
//static char* read_int(char* input, int* value) {
//  if (!is_digit(*input)) return NULL;
//  int temp = 0;
//  while (is_digit(*input)) {
//    temp = 10 * temp + (*input - '0');
//    input++;
//  }
//  *value = temp;
//  return input;
//}

char buffer[65536];

static unsigned int solve(int width, int height, int dx, int dy) {
  unsigned int count = 0;
  int x = 0;
  const int row_stride = dy * (width + 1);
  const int end = (width + 1) * height;
  for (int row_start = row_stride; row_start < end; row_start += row_stride) {
    x += dx;
    if (x >= width) x -= width;
    if (buffer[row_start + x] == '#') count++;
  }
  return count;
}

static unsigned int part1(int width, int height) {
  return solve(width, height, 3, 1);
}

static const int cases[][2] = {{1, 1}, {3, 1}, {5, 1}, {7, 1}, {1, 2}};
enum { num_cases = sizeof(cases) / sizeof(cases[0]) };
static unsigned int part2(int width, int height) {
  unsigned int total = 1;
  for (int i = 0; i < num_cases; i++) {
    total *= solve(width, height, cases[i][0], cases[i][1]);
  }
  return total;
}

int main() {
  // Read the input.
  int len = read(stdin, buffer, sizeof(buffer) - 1);
  if (len < 0) die("read");
  int width = 0;
  while (buffer[width] != '\n') width++;
  if (len % (width + 1)) die("bad input");
  const int height = len / (width + 1);
  print_int(part1(width, height));
  print_int(part2(width, height));
}
