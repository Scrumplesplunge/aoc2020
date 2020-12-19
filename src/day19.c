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

struct match_resume {
  void* data;
  const char* (*func)(void*, const char*);
};

// Given a rule number, an input string, and a match_resume, invoke the
// resumption function for every valid match of rule with some prefix of the
// input. If the resumption function returns non-NULL for any invocation, that
// value is immediately returned from match.
static const char* match(
    int rule, const char* input, struct match_resume resume);

struct match_sequence_state {
  unsigned char* parts;
  int num_parts;
  struct match_resume resume;
};

// Match a sequence against the input. The data parameter is a pointer to
// a match_sequence_state which holds the sequence.
static const char* match_sequence(void* data, const char* input) {
  struct match_sequence_state* state = data;
  if (state->num_parts == 0) {
    // The sequence is empty, so we match if and only if the resumption function
    // matches.
    return state->resume.func(state->resume.data, input);
  }
  // Invoke match() with the first part and use a resumption function that will
  // recursively invoke match_sequence for the remaining parts. This implements
  // the backtracking behaviour, allowing the parser to handle inputs that
  // cannot be handled by greedy parsing.
  struct match_sequence_state next_state = {
    .parts = state->parts + 1,
    .num_parts = state->num_parts - 1,
    .resume = state->resume,
  };
  return match(state->parts[0], input,
               (struct match_resume){.data = &next_state,
                                     .func = match_sequence});
}

// Try to match a given string to a rule. On a successful match, the position
// immediately after the match is returned. Otherwise, NULL is returned.
static const char* match(
    int rule, const char* input, struct match_resume resume) {
  struct rule* r = &rules[rule];
  if (r->num_sequences == 0) {
    // Literal character. If the character does not match, we don't match.
    // Otherwise, we match if the resumption function matches.
    return *input == r->literal ? resume.func(resume.data, input + 1) : NULL;
  } else {
    // Compound rule. Try each sequence in the union and return the first
    // succeeding match.
    for (int i = 0; i < r->num_sequences; i++) {
      struct match_sequence_state state = {
        .parts = r->sequences[i].parts,
        .num_parts = r->sequences[i].num_parts,
        .resume = resume,
      };
      const char* result = match_sequence(&state, input);
      if (result) return result;
    }
    return NULL;
  }
}

static const char* match_end(void* data, const char* input) {
  (void)data;
  return *input == '\0' ? input : NULL;
}

static int count_matches() {
  int count = 0;
  for (int i = 0; i < num_messages; i++) {
    const char* result = match(
        0, messages[i], (struct match_resume){.func = match_end});
    if (result && *result == '\0') count++;
  }
  return count;
}

int main() {
  read_input();
  print_int(count_matches());
  rules[8] = (struct rule){
    .num_sequences = 2,
    .sequences = {
      {.num_parts = 1, .parts = {42}},
      {.num_parts = 2, .parts = {42, 8}},
    },
  };
  rules[11] = (struct rule){
    .num_sequences = 2,
    .sequences = {
      {.num_parts = 2, .parts = {42, 31}},
      {.num_parts = 3, .parts = {42, 11, 31}},
    },
  };
  print_int(count_matches());
}
