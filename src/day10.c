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

enum { max_numbers = 128 };
unsigned short numbers[max_numbers];
int num_numbers;

static void sift_down(unsigned short* values, int num_values, int i) {
  const int x = values[i];
  while (1) {
    const int l = 2 * i + 1, r = l + 1;
    if (num_values <= l) break;
    const int max_child = r < num_values && values[l] < values[r] ? r : l;
    if (x >= values[max_child]) break;
    values[i] = values[max_child];
    i = max_child;
  }
  values[i] = x;
}

static void heapify(unsigned short* values, int num_values) {
  // Build a heap.
  for (int i = num_values - 1; i >= 0; i--) sift_down(values, num_values, i);
}

static int heap_pop(unsigned short* values, int num_values) {
  const int x = values[0];
  values[0] = values[num_values - 1];
  sift_down(values, num_values - 1, 0);
  return x;
}

static void sort(unsigned short* values, int num_values) {
  heapify(values, num_values);
  // Repeatedly pop.
  for (int i = num_values - 1; i >= 0; i--) {
    values[i] = heap_pop(values, i + 1);
  }
}

static void read_input() {
  char temp[512];
  const int len = read(STDIN_FILENO, temp, sizeof(temp));
  if (len <= 0) die("read");
  if (temp[len - 1] != '\n') die("newline");
  const char* i = temp;
  const char* const end = temp + len;
  while (i != end) {
    if (num_numbers == max_numbers) die("too many");
    i = read_int(i, &numbers[num_numbers++]);
    if (*i != '\n') die("line");
    i++;
  }
  sort(numbers, num_numbers);
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

unsigned long long arrangements[max_numbers];
static unsigned long long part2() {
  // arrangements[i] is the number of arrangements of adapters starting at the
  // power outlet and ending with device i.
  arrangements[0] = 1;
  for (int i = 1; i < num_numbers; i++) {
    arrangements[i] = numbers[i] <= 3;
    for (int j = i - 1; j >= 0; j--) {
      if (numbers[i] - numbers[j] > 3) break;
      arrangements[i] += arrangements[j];
    }
  }
  return arrangements[num_numbers - 1];
}

int main() {
  read_input();
  print_int64(part1());
  print_int64(part2());
}
