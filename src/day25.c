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

static unsigned mod_mul(unsigned a, unsigned b) {
  unsigned result = 0;
  for (int i = 0; i < 8; i++) {
    unsigned digits = (b >> (28 - 4 * i)) % 16;
    result = (16 * result + digits * a) % 20201227;
  }
  return result;
}

static unsigned mod_exp(unsigned a, unsigned b) {
  unsigned result = 1;
  for (int i = 0; i < 32; i++) {
    const _Bool bit = (b >> (31 - i)) % 2;
    result = mod_mul(result, result);
    if (bit) result = mod_mul(result, a);
  }
  return result;
}

int main() {
  char buffer[32];
  const int length = read(STDIN_FILENO, buffer, sizeof(buffer));
  if (length <= 0) die("read");
  if (buffer[length - 1] != '\n') die("newline");
  unsigned door, card;
  const char* i = buffer;
  i = read_int(i, &door);
  if (*i != '\n') die("line");
  i = read_int(i + 1, &card);
  if (*i != '\n') die("line");
  if (i + 1 - buffer != length) die("syntax");
  unsigned loop_size = 0, x = 1;
  // Find the loop size for the door.
  while (x != door) {
    loop_size++;
    x = x * 7 % 20201227;
  }
  // Compute the key.
  print_int(mod_exp(card, loop_size));
}
