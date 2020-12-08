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

static _Bool is_lower(char c) {
  return 'a' <= c && c <= 'z';
}

// Read a decimal integer from the string at input into value, returning the
// address of the first byte after the integer.
static char* read_int(char* input, int* value) {
  if (!is_digit(*input)) die("int");
  int temp = 0;
  while (is_digit(*input)) {
    temp = 10 * temp + (*input - '0');
    input++;
  }
  *value = temp;
  return input;
}

enum opcode {
  nop,
  acc,
  jmp,
};

struct operation {
  unsigned seen : 1;
  unsigned opcode : 2;
  int argument : 29;
};

enum { max_code_size = 1024 };
struct operation code[max_code_size];
int code_size;

#define C(x) ((unsigned)(unsigned char)(x))
#define KEY3(a, b, c) (C(a) | C(b) << 8 | C(c) << 16)

static void read_input() {
  char buffer[8192];
  int len = read(STDIN_FILENO, buffer, sizeof(buffer));
  if (len <= 0) die("read");
  if (buffer[len - 1] != '\n') die("newline");
  char* i = buffer;
  char* const end = buffer + len;
  while (i != end) {
    if (code_size == max_code_size) die("too long");
    struct operation* op = &code[code_size++];
    if (!is_lower(i[0]) || !is_lower(i[1]) || !is_lower(i[2]) || i[3] != ' ') {
      die("syntax");
    }
    switch (KEY3(i[0], i[1], i[2])) {
      case KEY3('n', 'o', 'p'):
        op->opcode = nop;
        break;
      case KEY3('a', 'c', 'c'):
        op->opcode = acc;
        break;
      case KEY3('j', 'm', 'p'):
        op->opcode = jmp;
        break;
      default:
        die("bad op");
    }
    i += 4;
    const _Bool negative = *i == '-';
    i++;
    int value;
    i = read_int(i, &value);
    op->argument = negative ? -value : value;
    if (*i != '\n') die("line");
    i++;
  }
}

static _Bool run(int* result) {
  for (int i = 0; i < code_size; i++) code[i].seen = 0;
  int i = 0;
  int accumulator = 0;
  while (i < code_size) {
    struct operation* op = &code[i];
    if (op->seen) {
      *result = accumulator;
      return 0;
    }
    op->seen = 1;
    switch (op->opcode) {
      case nop:
        i++;
        break;
      case acc:
        accumulator += op->argument;
        i++;
        break;
      case jmp:
        i += op->argument;
        break;
    }
  }
  *result = accumulator;
  return 1;
}

int main() {
  read_input();
  int part1;
  if (run(&part1)) die("part1 terminates");
  print_int(part1);
  for (int i = 0; i < code_size; i++) {
    int result;
    switch (code[i].opcode) {
      case nop:
        code[i].opcode = jmp;
        if (run(&result)) {
          print_int(result);
          return 0;
        }
        code[i].opcode = nop;
        break;
      case jmp:
        code[i].opcode = nop;
        if (run(&result)) {
          print_int(result);
          return 0;
        }
        code[i].opcode = jmp;
        break;
      default:
        break;
    }
  }
  die("no change works");
}
