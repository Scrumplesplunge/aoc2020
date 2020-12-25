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

// An entry stores a pair: 7^exponent = value (mod 20201227)
struct entry {
  unsigned value;
  unsigned exponent;
};

static void sift_down(struct entry* values, int num_values, int i) {
  const struct entry x = values[i];
  while (1) {
    const int l = 2 * i + 1, r = l + 1;
    if (num_values <= l) break;
    const int max_child =
        r < num_values && values[l].value < values[r].value ? r : l;
    if (x.value >= values[max_child].value) break;
    values[i] = values[max_child];
    i = max_child;
  }
  values[i] = x;
}

// Find x such that a^x = b (mod 20201227).
static unsigned mod_log(unsigned a, unsigned b) {
  enum { table_size = 5000 };
  struct entry table[table_size];
  // Populate the table values.
  unsigned e = 1;
  for (int i = 0; i < table_size; i++) {
    table[i] = (struct entry){.value = e, .exponent = i};
    e = a * e % 20201227;
  }
  // Sort the table.
  for (int i = table_size - 1; i >= 0; i--) sift_down(table, table_size, i);
  for (int i = table_size - 1; i >= 0; i--) {
    const struct entry x = table[0];
    table[0] = table[i];
    sift_down(table, i, 0);
    table[i] = x;
  }
  // Search for the exponent.
  const unsigned factor = mod_exp(a, 20201227 - 1 - table_size);
  unsigned exponent = 0;
  e = b;
  while (1) {
    // Find the value in the table.
    unsigned i = 0, j = table_size;
    while (j - i > 1) {
      unsigned mid = i + (j - i) / 2;
      if (table[mid].value <= e) {
        i = mid;
      } else {
        j = mid;
      }
    }
    if (table[i].value == e) return exponent + table[i].exponent;
    e = mod_mul(e, factor);
    exponent += table_size;
  }
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
  // Find the loop size for the door.
  const unsigned loop_size = mod_log(7, door);
  // Compute the key.
  print_int(mod_exp(card, loop_size));
}
