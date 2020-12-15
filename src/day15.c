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

unsigned spoken[30000001];
int main() {
  char buffer[128];
  const int length = read(STDIN_FILENO, buffer, sizeof(buffer));
  if (length <= 0) die("read");
  if (buffer[length - 1] != '\n') die("newline");
  const char* i = buffer;
  unsigned turn = 0;
  unsigned last_number;
  memset(spoken, -1, sizeof(spoken));
  while (1) {
    ++turn;
    i = read_int(i, &last_number);
    spoken[last_number] = turn;
    if (*i == '\n') break;
    if (*i != ',') die("comma");
    i++;
  }
  while (turn < 2020) {
    const unsigned answer =
        spoken[last_number] != -1 ? turn - spoken[last_number] : 0;
    spoken[last_number] = turn;
    turn++;
    last_number = answer;
  }
  print_int(last_number);
  while (turn < 30000000) {
    const unsigned answer =
        spoken[last_number] != -1 ? turn - spoken[last_number] : 0;
    spoken[last_number] = turn;
    turn++;
    last_number = answer;
  }
  print_int(last_number);
}
