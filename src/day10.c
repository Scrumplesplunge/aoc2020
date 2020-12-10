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
static const char* read_int(const char* input, unsigned short* value) {
  if (!is_digit(*input)) die("int");
  unsigned short temp = 0;
  while (is_digit(*input)) {
    temp = 10 * temp + (*input - '0');
    input++;
  }
  *value = temp;
  return input;
}

enum { max_numbers = 256 };
unsigned short numbers[max_numbers];
int num_numbers;

static void read_input() {
  char buffer[512];
  const int len = read(STDIN_FILENO, buffer, sizeof(buffer));
  if (len <= 0) die("read");
  if (buffer[len - 1] != '\n') die("newline");
  const char* i = buffer;
  const char* const end = buffer + len;
  while (i != end) {
    if (num_numbers == max_numbers) die("too many");
    i = read_int(i, &numbers[num_numbers++]);
    if (*i != '\n') die("line");
    i++;
  }
  for (int i = 0; i < num_numbers; i++) {
    int min = i;
    for (int j = i; j < num_numbers; j++) {
      if (numbers[j] < numbers[min]) min = j;
    }
    unsigned short temp = numbers[i];
    numbers[i] = numbers[min];
    numbers[min] = temp;
  }
}

static int part1() {
  int counts[4] = {0};
  int value = 0;
  for (int i = 0; i < num_numbers; i++) {
    int delta = numbers[i] - value;
    if (delta > 3) die("bad input");
    value = numbers[i];
    counts[delta]++;
  }
  counts[3]++;
  return counts[1] * counts[3];
}

unsigned long long arrangements_buffer[3 * max_numbers + 10];
static unsigned long long part2() {
  // arrangements[i] is the number of arrangements of adapters resulting in
  // a joltage level of i. It is offset by 2 into the buffer so that the loop
  // doesn't have to have special cases for the first few elements.
  unsigned long long* arrangements = arrangements_buffer + 2;
  arrangements[0] = 1;
  for (int i = 0; i < num_numbers; i++) {
    const int x = numbers[i];
    arrangements[x] =
        arrangements[x - 1] + arrangements[x - 2] + arrangements[x - 3];
  }
  return arrangements[numbers[num_numbers - 1]];
}

int main() {
  read_input();
  print_int64(part1());
  print_int64(part2());
}
