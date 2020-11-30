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

__attribute__((noreturn)) static void die(const char* message) {
  write(stderr, message, strlen(message));
  write(stderr, "\n", 1);
  exit(1);
}

static void print_int(int x) {
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

static _Bool is_digit(char c) {
  return '0' <= c && c <= '9';
}

static _Bool is_whitespace(char c) {
  return c == ' ' || c == '\n';
}

static const char* skip_whitespace(const char* input) {
  while (is_whitespace(*input)) input++;
  return input;
}

static const char* read_int(const char* input, int* value) {
  if (!is_digit(*input)) return NULL;
  int temp = 0;
  while ('0' <= *input && *input <= '9') {
    temp = 10 * temp + (*input - '0');
    input++;
  }
  *value = temp;
  return input;
}

char buffer[1024];

int fuel(int mass) {
  int x = mass / 3 - 2;
  return x > 0 ? x + fuel(x) : 0;
}

int main() {
  int len = read(stdin, buffer, sizeof(buffer) - 1);
  if (len == -1) die("read");
  int part_1 = 0, part_2 = 0;
  const char* i = buffer;
  const char* const end = buffer + len;
  while (i != end) {
    i = skip_whitespace(i);
    if (i == end) break;
    int x;
    i = read_int(skip_whitespace(i), &x);
    if (i == NULL) die("bad input");
    part_1 += x / 3 - 2;
    part_2 += fuel(x);
  }
  print_int(part_1);
  print_int(part_2);
}
