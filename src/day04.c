// Exit the program with a given error message.
__attribute__((noreturn)) static void die(const char* message) {
  write(STDERR_FILENO, message, strlen(message));
  write(STDERR_FILENO, "\n", 1);
  exit(1);
}

// Print an integer in decimal, followed by a newline.
static void print_int(int x) {
  char buffer[16];
  buffer[15] = '\n';
  int i = 15;
  do {
    --i;
    buffer[i] = '0' + (x % 10);
    x /= 10;
  } while (x);
  write(STDOUT_FILENO, buffer + i, 16 - i);
}

static _Bool is_lower(char c) {
  return 'a' <= c && c <= 'z';
}

static _Bool is_whitespace(char c) {
  return c == ' ' || c == '\n';
}

struct entry {
  const char* key;
  const char* value;
};

// Read a single passport entry into the provided struct. `key` and `value` will
// be updated to point at null-terminated strings for the key and value
// respectively. The `key` will always be of length 3, but the value can have
// arbitrary length.
char* read_entry(char* i, struct entry* out) {
  out->key = i;
  if (*i == '\n') {
    *out = (struct entry){0};
    return i + 1;
  }
  if (!is_lower(i[0]) || !is_lower(i[1]) || !is_lower(i[2]) || i[3] != ':') {
    die("syntax");
  }
  i[3] = '\0';
  i += 4;
  out->value = i;
  while (!is_whitespace(*i)) i++;
  *i = '\0';
  return i + 1;
}

static char buffer[32768];

int main() {
  int len = read(STDIN_FILENO, buffer, sizeof(buffer));
  if (len < 0) die("read");
  char* i = buffer;
  char* const end = buffer + len;
  int count = 0;
  while (i < end) {
    // Iterate over all entries in a single passport.
    int num_entries = 0;
    struct entry entry;
    while (i < end) {
      i = read_entry(i, &entry);
      const char* k = entry.key;
      if (!k) break;
      // Don't count cid entries.
      if (k[0] == 'c' && k[1] == 'i' && k[2] == 'd') continue;
      num_entries++;
    }
    // If a passport has 7 entries and we assume that there are no duplicate
    // keys, it must have all the required fields.
    if (num_entries == 7) count++;
  }
  print_int(count);
}
