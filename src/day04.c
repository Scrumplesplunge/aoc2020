#include "util/die.h"
#include "util/print_int.h"
#include "util/read_int.h"
#include "util/is_lower.h"

static bool is_whitespace(char c) {
  return c == ' ' || c == '\n';
}

static bool is_hex(char c) {
  return is_digit(c) || ('a' <= c && c <= 'f');
}

#define KEY3(a, b, c) ((unsigned)(a) | (unsigned)(b) << 8 | (unsigned)(c) << 16)
#define KEY(s) KEY3(s[0], s[1], s[2])
enum key {
  invalid = 0,
  byr = KEY3('b', 'y', 'r'),
  iyr = KEY3('i', 'y', 'r'),
  eyr = KEY3('e', 'y', 'r'),
  hgt = KEY3('h', 'g', 't'),
  hcl = KEY3('h', 'c', 'l'),
  ecl = KEY3('e', 'c', 'l'),
  pid = KEY3('p', 'i', 'd'),
  cid = KEY3('c', 'i', 'd'),
};

struct entry {
  enum key key;
  const char* value;
  int value_length;
};

// Read a single passport entry into the provided struct. `key` and `value` will
// be updated to point at null-terminated strings for the key and value
// respectively. The `key` will always be of length 3, but the value can have
// arbitrary length. If there are no more entries in this passport, the entry
// key will be set to 0.
static char* read_entry(char* i, struct entry* out) {
  const char* key = i;
  if (*i == '\n') {
    *out = (struct entry){0};
    return i + 1;
  }
  if (!is_lower(i[0]) || !is_lower(i[1]) || !is_lower(i[2]) || i[3] != ':') {
    die("syntax");
  }
  out->key = KEY(key);
  i += 4;
  out->value = i;
  while (!is_whitespace(*i)) i++;
  out->value_length = i - out->value;
  *i = '\0';
  return i + 1;
}

static char buffer[32768];

static bool check_year(struct entry* e, unsigned min, unsigned max) {
  if (e->value_length != 4) return false;
  unsigned value;
  if (read_int(e->value, &value) != e->value + 4) return false;
  return min <= value && value <= max;
}

static bool check_height(struct entry* e) {
  if (e->value_length < 3) return false;
  const char* const unit = e->value + e->value_length - 2;
  const bool is_cm = unit[0] == 'c' && unit[1] == 'm';
  const bool is_in = unit[0] == 'i' && unit[1] == 'n';
  unsigned value;
  if (read_int(e->value, &value) != unit) return false;
  if (is_cm) {
    return 150 <= value && value <= 193;
  } else if (is_in) {
    return 59 <= value && value <= 76;
  } else {
    return false;
  }
}

static bool check_hair(struct entry* e) {
  if (e->value_length != 7) return false;
  if (e->value[0] != '#') return false;
  for (int i = 1; i < 7; i++) {
    if (!is_hex(e->value[i])) return false;
  }
  return true;
}

static bool check_eyes(struct entry* e) {
  if (e->value_length != 3) return false;
  switch (KEY(e->value)) {
    case KEY3('a', 'm', 'b'):
    case KEY3('b', 'l', 'u'):
    case KEY3('b', 'r', 'n'):
    case KEY3('g', 'r', 'y'):
    case KEY3('g', 'r', 'n'):
    case KEY3('h', 'z', 'l'):
    case KEY3('o', 't', 'h'):
      return true;
    default:
      return false;
  }
}

static bool check_pid(struct entry* e) {
  if (e->value_length != 9) return false;
  unsigned x;
  return read_int(e->value, &x) == e->value + 9;
}

static bool part2_valid(struct entry* e) {
  switch (e->key) {
    case byr: return check_year(e, 1920, 2002);
    case iyr: return check_year(e, 2010, 2020);
    case eyr: return check_year(e, 2020, 2030);
    case hgt: return check_height(e);
    case hcl: return check_hair(e);
    case ecl: return check_eyes(e);
    case pid: return check_pid(e);
    case cid: return false;
    default: return false;
  }
}

struct passport_validity {
  int part1;
  int part2;
};

static char* check_passport(
    char* i, char* const end, struct passport_validity* out) {
  int part1 = 0, part2 = 0;
  while (i < end) {
    struct entry entry;
    i = read_entry(i, &entry);
    if (!entry.key) break;
    part1 += entry.key != cid;
    part2 += part2_valid(&entry);
  }
  out->part1 += part1 == 7;
  out->part2 += part2 == 7;
  return i;
}

int main() {
  int len = read(STDIN_FILENO, buffer, sizeof(buffer));
  if (len <= 0) die("read");
  if (buffer[len - 1] != '\n') die("newline");
  char* i = buffer;
  char* const end = buffer + len;
  struct passport_validity num_valid = {0};
  while (i < end) {
    i = check_passport(i, end, &num_valid);
  }
  print_int(num_valid.part1);
  print_int(num_valid.part2);
}
