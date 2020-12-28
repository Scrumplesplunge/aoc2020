#include "util/die.h"
#include "util/print_int.h"

static _Bool is_digit(char c) {
  return '0' <= c && c <= '9';
}

// Read a decimal integer from the string at input into value, returning the
// address of the first byte after the integer.
static char* read_int(char* input, int* value) {
  if (!is_digit(*input)) return NULL;
  int temp = 0;
  while (is_digit(*input)) {
    temp = 10 * temp + (*input - '0');
    input++;
  }
  *value = temp;
  return input;
}

static char buffer[65536];

struct entry {
  unsigned char min, max;
  char c;
  const char* password;
};

enum { max_entries = 1000 };
static struct entry entries[max_entries];
static int num_entries;

static int part1(void) {
  int total = 0;
  for (int i = 0; i < num_entries; i++) {
    int count = 0;
    for (const char* j = entries[i].password; *j; j++) {
      if (*j == entries[i].c) count++;
    }
    if (entries[i].min <= count && count <= entries[i].max) total++;
  }
  return total;
}

static int part2(void) {
  int total = 0;
  for (int i = 0; i < num_entries; i++) {
    struct entry* e = &entries[i];
    int len = strlen(e->password);
    if (len < e->min || len < e->max) die("bad index");
    const _Bool first_matches = e->password[e->min - 1] == e->c;
    const _Bool second_matches = e->password[e->max - 1] == e->c;
    if (first_matches != second_matches) total++;
  }
  return total;
}

int main() {
  // Parse the input into `entries`.
  int len = read(STDIN_FILENO, buffer, sizeof(buffer) - 1);
  if (len < 0) die("read");
  char* i = buffer;
  char* const end = buffer + len;
  while (i < end) {
    if (num_entries == max_entries) die("too many");
    int min, max;
    i = read_int(i, &min);
    if (i == NULL || min > 255) die("lower bound");
    if (*i != '-') die("hyphen");
    i = read_int(i + 1, &max);
    if (i == NULL || min > 255) die("upper bound");
    if (*i != ' ') die("space");
    char c = i[1];
    if (i[2] != ':' || i[3] != ' ') die("colon");
    i += 4;
    const char* password = i;
    while (i != end && *i != '\n') i++;
    *i = '\0';
    entries[num_entries++] =
        (struct entry){.min = min, .max = max, .c = c, .password = password};
    i++;
  }
  print_int(part1());
  print_int(part2());
}
