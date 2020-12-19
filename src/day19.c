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
static char* read_int(char* input, unsigned* value) {
  if (!is_digit(*input)) die("int");
  unsigned temp = 0;
  while (is_digit(*input)) {
    temp = 10 * temp + (*input - '0');
    input++;
  }
  *value = temp;
  return input;
}

enum { max_parts = 3, max_sequences = 2, max_rules = 256, max_messages = 1024 };
struct sequence {
  unsigned char num_parts;
  unsigned char parts[max_parts];
};

struct rule {
  // If num_sequences is 0, the rule is a literal. Otherwise, it is a union of
  // sequences with the given number of components.
  unsigned char num_sequences;
  union {
    char literal;
    struct sequence sequences[max_sequences];
  };
};

char buffer[32768];
struct rule rules[max_rules];
const char* messages[max_messages];
int num_messages;

static void read_input() {
  char buffer[32768];
  const int length = read(STDIN_FILENO, buffer, sizeof(buffer));
  if (length <= 0) die("read");
  if (buffer[length - 1] != '\n') die("newline");
  char* i = buffer;
  char* end = buffer + length;
  // Parse the rules.
  while (i != end && *i != '\n') {
    unsigned id;
    i = read_int(i, &id);
    if (*i != ':') die("colon");
    if (id >= max_rules) die("too many rules");
    struct rule* r = &rules[id];
    // Skip ": "
    i += 2;
    if (*i == '"') {
      r->literal = i[1];
      i += 3;
    } else {
      // Iterate over each concat sequence of the rule.
      do {
        if (r->num_sequences == max_sequences) die("too many sequences");
        struct sequence* s = &r->sequences[r->num_sequences++];
        // Iterate over each component of a concat sequence.
        while (*i != '|') {
          if (s->num_parts == max_parts) die("too many parts");
          unsigned x;
          i = read_int(i, &x);
          if (x >= max_rules) die("bad reference");
          s->parts[s->num_parts++] = x;
          if (*i == '\n') break;
          if (*i != ' ') die("syntax");
          i++;
        }
        if (*i == '\n') break;
        if (*i != '|') die("syntax");
        i += 2;
      } while (1);
    }
    if (*i != '\n') die("line");
    i++;
  }
  if (*i != '\n') die("syntax");
  i++;
  // Turn each input string into a separate null terminated string.
  while (i != end) {
    if (num_messages == max_messages) die("too many messages");
    messages[num_messages++] = i;
    while (*i != '\n') i++;
    *i = '\0';
    i++;
  }
}

static const char* match(int rule, const char* input);

static const char* match_sequence(struct sequence* s, const char* input) {
  for (int i = 0; i < s->num_parts; i++) {
    const char* result = match(s->parts[i], input);
    if (!result) return NULL;
    input = result;
  }
  return input;
}

// Try to match a given string to a rule. On a successful match, the position
// immediately after the match is returned. Otherwise, NULL is returned.
static const char* match(int rule, const char* input) {
  struct rule* r = &rules[rule];
  if (r->num_sequences == 0) {
    return *input == r->literal ? input + 1 : NULL;
  } else {
    // TODO: Replace this with a shift-reduce parser. This will degrade horribly
    // for some rules due to backtracking and it can't handle ambiguous
    // prefixes.
    for (int i = 0; i < r->num_sequences; i++) {
      const char* result = match_sequence(&r->sequences[i], input);
      if (result) return result;
    }
    return NULL;
  }
}

static int part1() {
  int count = 0;
  for (int i = 0; i < num_messages; i++) {
    const char* result = match(0, messages[i]);
    if (result && *result == '\0') count++;
  }
  return count;
}

int main() {
  read_input();
  print_int(part1());
}
