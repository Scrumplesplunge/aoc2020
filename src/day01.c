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
  while (is_digit(*input)) {
    temp = 10 * temp + (*input - '0');
    input++;
  }
  *value = temp;
  return input;
}

char buffer[1024];
enum { max_numbers = 200 };
int numbers[max_numbers];
int n;
char set[2021];

static void read_input() {
  int len = read(stdin, buffer, sizeof(buffer) - 1);
  if (len == -1) die("read");
  const char* i = buffer;
  const char* const end = buffer + len;
  while (i != end) {
    i = skip_whitespace(i);
    if (i == end) break;
    if (n == max_numbers) die("too many");
    i = read_int(skip_whitespace(i), &numbers[n]);
    if (i == NULL) die("bad input");
    if (numbers[n] > 2020) die("too large");
    set[numbers[n]] = 1;
    n++;
  }
}

static int part1() {
  for (int i = 0; i < n; i++) {
    if (set[2020 - numbers[i]]) {
      return numbers[i] * (2020 - numbers[i]);
    }
  }
  die("not found");
}

static int part2() {
  for (int i = 0; i < n; i++) {
    int remaining = 2020 - numbers[i];
    for (int j = i + 1; j < n; j++) {
      if (numbers[j] > remaining) continue;
      const int num_k = remaining - numbers[j];
      if (set[num_k]) {
        return numbers[i] * numbers[j] * num_k;
      }
    }
  }
  die("not found");
}

int main() {
  read_input();
  print_int(part1());
  print_int(part2());
}
