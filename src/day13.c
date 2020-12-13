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

static _Bool is_digit(char c) {
  return '0' <= c && c <= '9';
}

// Read a decimal integer from the string at input into value, returning the
// address of the first byte after the integer.
static const char* read_int(const char* input, unsigned* value) {
  if (!is_digit(*input)) die("int");
  unsigned temp = 0;
  while (is_digit(*input)) {
    temp = 10 * temp + (*input - '0');
    input++;
  }
  *value = temp;
  return input;
}

// Round x up to the next multiple of k.
static unsigned round_up(unsigned x, unsigned k) {
  return (x + k - 1) / k * k;
}

static int part1(const char* input) {
  unsigned earliest_departure;
  enum { max_buses = 128 };
  unsigned buses[max_buses];
  int num_buses = 0;
  input = read_int(input, &earliest_departure);
  if (*input != '\n') die("bad input");
  do {
    input++;
    if (*input == 'x') {
      input++;
    } else {
      if (num_buses == max_buses) die("too large");
      input = read_int(input, &buses[num_buses++]);
    }
  } while (*input == ',');
  if (*input != '\n') die("bad input");
  unsigned bus = 0, time = round_up(earliest_departure, buses[0]);
  for (int i = 1; i < num_buses; i++) {
    const unsigned new_time = round_up(earliest_departure, buses[i]);
    if (new_time < time) {
      bus = i;
      time = new_time;
    }
  }
  return buses[bus] * (time - earliest_departure);
}

static char buffer[256];
int main() {
  int length = read(STDIN_FILENO, buffer, sizeof(buffer) - 1);
  if (length <= 0) die("read");
  if (buffer[length - 1] != '\n') die("newline");
  buffer[length] = '\0';
  print_int(part1(buffer));
}
