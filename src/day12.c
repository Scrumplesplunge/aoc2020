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

enum action {
  north,
  south,
  east,
  west,
  left,
  right,
  forward,
};

struct instruction {
  enum action action;
  unsigned short value;
};

enum { max_instructions = 1024 };
static struct instruction instructions[max_instructions];
static int num_instructions;

static enum action parse_action(char c) {
  switch (c) {
    case 'N': return north;
    case 'S': return south;
    case 'E': return east;
    case 'W': return west;
    case 'L': return left;
    case 'R': return right;
    case 'F': return forward;
    default: die("bad action");
  }
}

static void read_input() {
  char temp[8192];
  const int length = read(STDIN_FILENO, temp, sizeof(temp));
  if (length <= 0) die("read");
  if (temp[length - 1] != '\n') die("newline");
  const char* i = temp;
  const char* const end = temp + length;
  while (i != end) {
    if (num_instructions == max_instructions) die("too many");
    struct instruction* instruction = &instructions[num_instructions++];
    instruction->action = parse_action(*i);
    unsigned short value;
    i = read_int(i + 1, &value);
    if (instruction->action == left || instruction->action == right) {
      // We'll go off the grid if the angles aren't multiples of 90, and it is
      // easier to work with the quotient anyway.
      if (value == 0 || value >= 360 || value % 90 != 0) die("angle");
      value /= 90;
    }
    instruction->value = value;
    if (*i != '\n') die("line");
    i++;
  }
}

struct vec { int x, y; };

static struct vec rotate_left(unsigned char amount, struct vec v) {
  switch (amount) {
    case 1: return (struct vec){v.y, -v.x};
    case 2: return (struct vec){-v.x, -v.y};
    case 3: return (struct vec){-v.y, v.x};
  }
  die("bug");
}

static struct vec rotate_right(unsigned char amount, struct vec v) {
  switch (amount) {
    case 1: return (struct vec){-v.y, v.x};
    case 2: return (struct vec){-v.x, -v.y};
    case 3: return (struct vec){v.y, -v.x};
  }
  die("bug");
}

static int part1() {
  struct vec position = {0, 0}, direction = {1, 0};
  for (int i = 0; i < num_instructions; i++) {
    switch (instructions[i].action) {
      case north:
        position.y -= instructions[i].value;
        break;
      case south:
        position.y += instructions[i].value;
        break;
      case east:
        position.x += instructions[i].value;
        break;
      case west:
        position.x -= instructions[i].value;
        break;
      case forward:
        position.x += direction.x * instructions[i].value;
        position.y += direction.y * instructions[i].value;
        break;
      case left:
        direction = rotate_left(instructions[i].value, direction);
        break;
      case right:
        direction = rotate_right(instructions[i].value, direction);
        break;
    }
  }
  const int x = position.x < 0 ? -position.x : position.x;
  const int y = position.y < 0 ? -position.y : position.y;
  return x + y;
}

int main() {
  read_input();
  print_int(part1());
}