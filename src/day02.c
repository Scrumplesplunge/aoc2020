#include "util/die.h"
#include "util/print_int.h"
#include "util/read_int.h"
#include "util/strlen.h"

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
    const bool first_matches = e->password[e->min - 1] == e->c;
    const bool second_matches = e->password[e->max - 1] == e->c;
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
    unsigned min, max;
    i = (char*)read_int(i, &min);
    if (i == NULL || min > 255) die("lower bound");
    if (*i != '-') die("hyphen");
    i = (char*)read_int(i + 1, &max);
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
