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
static char* read_int(char* input, unsigned short* value) {
  if (!is_digit(*input)) die("int");
  unsigned short temp = 0;
  while (is_digit(*input)) {
    temp = 10 * temp + (*input - '0');
    input++;
  }
  *value = temp;
  return input;
}

struct range {
  unsigned short min, max;
};

struct field {
  const char* name;
  struct range ranges[2];
};

enum { max_fields = 20 };
struct field fields[max_fields];
int num_fields;

struct ticket {
  unsigned short values[max_fields];
};

enum { max_tickets = 512 };
struct ticket tickets[max_tickets];
int num_tickets;

// Read a ticket into the given struct. Returns a pointer to the address
// immediately after the ticket in the input. Dies if the ticket is invalid.
static char* read_ticket(char* i, struct ticket* t) {
  int num_values = 0;
  while (1) {
    if (num_values == num_fields) die("too many values");
    i = read_int(i, &t->values[num_values++]);
    if (*i != ',') break;
    i++;
  }
  if (*i != '\n') die("bad ticket");
  return i + 1;
}

// Returns true if the x is a valid value for field f.
static _Bool valid(struct field* f, int x) {
  return (f->ranges[0].min <= x && x <= f->ranges[0].max) ||
         (f->ranges[1].min <= x && x <= f->ranges[1].max);
}

// Returns true if x is an invalid value for all fields.
static _Bool invalid_value(int x) {
  int valid_fields = 0;
  for (int j = 0; j < num_fields; j++) {
    if (valid(&fields[j], x)) valid_fields++;
  }
  return valid_fields == 0;
}

// Returns true if t is not a valid ticket.
static _Bool invalid_ticket(struct ticket* t) {
  for (int i = 0; i < num_fields; i++) {
    if (invalid_value(t->values[i])) return 1;
  }
  return 0;
}

static int part1() {
  // Sum up the values of all invalid values.
  int total = 0;
  for (int i = 0; i < num_tickets; i++) {
    for (int j = 0; j < num_fields; j++) {
      const int x = tickets[i].values[j];
      if (invalid_value(x)) total += x;
    }
  }
  return total;
}

// valid_pairs[i][f] is false if index i cannot possibly represent field f.
_Bool valid_pairs[max_fields][max_fields];

// Given a partial assignment result[0..num_fields) with -1 for unassigned
// fields, and a set of taken indices where taken[i] is true if result already
// has a field assigned to i, returns the field index of the least ambiguous
// field.
int least_ambiguous(signed char* result, _Bool* taken) {
  // Check for one field that is unambiguous.
  int ambiguity = num_fields + 1;
  int field = -1;
  for (int f = 0; f < num_fields; f++) {
    if (result[f] != -1) continue;  // Skip fields that are already assigned.
    // Check that exactly one value index is valid.
    int num_valid = 0;
    for (int i = 0; i < num_fields; i++) {
      if (valid_pairs[i][f] && !taken[i]) num_valid++;
    }
    if (num_valid < ambiguity) {
      ambiguity = num_valid;
      field = f;
    }
  }
  return field;
}

// Given a partial assignment result[0..num_fields) with -1 for unassigned
// fields, finds an assignment for the unassigned fields such that all the
// validity constraints in `valid_pairs` are satisfied. Returns true if
// successful, or false if no such assignment can be found.
static _Bool deduce_fields(signed char* result) {
  _Bool taken[max_fields] = {0};
  _Bool all_taken = 1;
  for (int f = 0; f < num_fields; f++) {
    if (result[f] != -1) {
      taken[result[f]] = 1;
    } else {
      all_taken = 0;
    }
  }
  if (all_taken) return 1;  // All fields have been assigned, we are done.
  // Find the least ambiguous field and try each possible assignment for it.
  const int f = least_ambiguous(result, taken);
  if (f == -1) return 0;
  for (int i = 0; i < num_fields; i++) {
    if (valid_pairs[i][f] && !taken[i]) {
      result[f] = i;
      if (deduce_fields(result)) return 1;
    }
  }
  return 0;
}

static unsigned long long part2() {
  // Eliminate all conflicting entries.
  memset(valid_pairs, 1, sizeof(valid_pairs));
  for (int i = 0; i < num_tickets; i++) {
    if (invalid_ticket(&tickets[i])) continue;
    // For each value index.
    for (int j = 0; j < num_fields; j++) {
      // For each field.
      for (int k = 0; k < num_fields; k++) {
        if (!valid(&fields[k], tickets[i].values[j])) {
          valid_pairs[j][k] = 0;
        }
      }
    }
  }
  // indices[i] is the value index for field i.
  signed char indices[max_fields];
  memset(indices, -1, sizeof(indices));
  if (!deduce_fields(indices)) die("cannot deduce mapping");
  // Accumulate the departure fields.
  int count = 0;
  unsigned long long total = 1;
  for (int i = 0; i < num_fields; i++) {
    if (strncmp(fields[i].name, "departure ", 10) != 0) continue;
    total *= tickets[0].values[indices[i]];
    count++;
  }
  if (count != 6) die("wrong number of fields");
  return total;
}

int main() {
  char buffer[32768];
  const int length = read(STDIN_FILENO, buffer, sizeof(buffer));
  if (length <= 0) die("read");
  if (buffer[length - 1] != '\n') die("newline");
  char* i = buffer;
  while (1) {
    if (*i == '\n') break;
    if (num_fields == max_fields) die("too many fields");
    struct field* f = &fields[num_fields++];
    f->name = i;
    while (*i != ':') i++;
    *i = '\0';
    i = read_int(i + 2, &f->ranges[0].min);
    if (*i != '-') die("bad");
    i = read_int(i + 1, &f->ranges[0].max);
    // " or "
    i = read_int(i + 4, &f->ranges[1].min);
    if (*i != '-') die("bad");
    i = read_int(i + 1, &f->ranges[1].max);
    if (*i != '\n') die("line");
    i++;
  }
  i++;
  i[12] = '\0';
  if (strcmp(i, "your ticket:") != 0) die("syntax");
  i += 13;
  i = read_ticket(i, &tickets[num_tickets++]);
  i++;
  i[15] = '\0';
  if (strcmp(i, "nearby tickets:") != 0) die("syntax");
  i += 16;
  const char* const end = buffer + length;
  while (i != end) i = read_ticket(i, &tickets[num_tickets++]);
  print_int64(part1());
  print_int64(part2());
}
