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

// Print an integer in decimal, followed by a newline.
static void print_int64(unsigned long long x) {
  char buffer[24] = {[23] = '\n'};
  // Compute the decimal format one bit at a time by doubling and carrying BCD.
  for (int i = 63; i >= 0; i--) {
    for (int j = 0; j < 23; j++) buffer[j] *= 2;
    if ((x >> i) & 1) buffer[22]++;
    char carry = 0;
    for (int j = 22; j >= 0; j--) {
      char temp = buffer[j] + carry;
      buffer[j] = temp % 10;
      carry = temp / 10;
    }
  }
  // Compute the most significant digit and truncate the output.
  int i = 0;
  while (buffer[i] == 0) i++;
  const int start = i < 22 ? i : 22;
  for (int j = start; j < 23; j++) buffer[j] += '0';
  write(stdout, buffer + start, 24 - start);
}

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
static unsigned long long part2(int width, int height) {
  unsigned long long total = 1;
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
  print_int64(part1(width, height));
  print_int64(part2(width, height));
}
