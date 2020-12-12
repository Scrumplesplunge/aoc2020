// Exit the program with a given error message.
__attribute__((noreturn)) static void die(const char* message) {
  write(STDERR_FILENO, message, strlen(message));
  write(STDERR_FILENO, "\n", 1);
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
  write(STDOUT_FILENO, buffer + start, 24 - start);
}

static _Bool is_digit(char c) {
  return '0' <= c && c <= '9';
}

// Read a decimal integer from the string at input into value, returning the
// address of the first byte after the integer.
static const char* read_int(const char* input, unsigned long long* value) {
  if (!is_digit(*input)) die("int");
  unsigned long long temp = 0;
  while (is_digit(*input)) {
    temp = 10 * temp + (*input - '0');
    input++;
  }
  *value = temp;
  return input;
}

enum { max_numbers = 1024 };
static char buffer[16384];
static unsigned char prelude_size = 25;
static unsigned long long numbers[max_numbers];
static int num_numbers;

static void read_input() {
  const int len = read(STDIN_FILENO, buffer, sizeof(buffer));
  if (len <= 0) die("read");
  if (buffer[len - 1] != '\n') die("newline");
  const char* i = buffer;
  const char* const end = buffer + len;
  // Processing for the test case: we look for `#N` on the first line. If
  // present, we use `N` as the prelude_size instead of 25.
  if (*i == '#') {
    i++;
    unsigned long long temp;
    i = read_int(i, &temp);
    if (*i != '\n') die("syntax");
    i++;
    if (temp > 25) die("prelude too big");
    prelude_size = temp;
  }
  while (i != end) {
    i = read_int(i, &numbers[num_numbers++]);
    if (*i != '\n') die("line");
    i++;
  }
  if (num_numbers < prelude_size) die("too small");
}

static _Bool has_sum(unsigned long long window[25],
                     unsigned long long value) {
  for (int i = 0; i < prelude_size; i++) {
    for (int j = i + 1; j < prelude_size; j++) {
      if (value == window[i] + window[j]) return 1;
    }
  }
  return 0;
}

static unsigned long long part1() {
  unsigned long long window[25];
  // Read the prelude.
  for (int i = 0; i < prelude_size; i++) window[i] = numbers[i];
  // Search for the value.
  for (int i = prelude_size; i < num_numbers; i++) {
    if (!has_sum(window, numbers[i])) {
      return numbers[i];
    }
    window[i % prelude_size] = numbers[i];
  }
  die("not found");
}

static unsigned long long part2(unsigned long long key) {
  unsigned long long sum = 0;
  int first = 0, last = 0;
  while (1) {
    if (sum == key) break;
    if (sum < key) {
      if (last == num_numbers) die("not found");
      sum += numbers[last++];
    } else {
      sum -= numbers[first++];
    }
  }
  unsigned long long min = -1, max = 0;
  for (int i = first; i < last; i++) {
    if (numbers[i] < min) min = numbers[i];
    if (numbers[i] > max) max = numbers[i];
  }
  return min + max;
}

int main() {
  read_input();
  const unsigned long long key = part1();
  print_int64(key);
  print_int64(part2(key));
}
