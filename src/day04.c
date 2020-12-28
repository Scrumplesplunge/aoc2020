#include "util/die.h"

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

static _Bool is_digit(char c) {
  return '0' <= c && c <= '9';
}

static _Bool is_lower(char c) {
  return 'a' <= c && c <= 'z';
}

static _Bool is_whitespace(char c) {
  return c == ' ' || c == '\n';
}

static _Bool is_hex(char c) {
  return is_digit(c) || ('a' <= c && c <= 'f');
}

// Read a decimal integer from the string at input into value, returning the
// address of the first byte after the integer.
static const char* read_int(const char* input, int* value) {
  if (!is_digit(*input)) return NULL;
  int temp = 0;
  while (is_digit(*input)) {
    temp = 10 * temp + (*input - '0');
    input++;
  }
  *value = temp;
  return input;
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

static _Bool check_year(struct entry* e, int min, int max) {
  if (e->value_length != 4) return 0;
  int value;
  if (read_int(e->value, &value) != e->value + 4) return 0;
  return min <= value && value <= max;
}

static _Bool check_height(struct entry* e) {
  if (e->value_length < 3) return 0;
  const char* const unit = e->value + e->value_length - 2;
  const _Bool is_cm = unit[0] == 'c' && unit[1] == 'm';
  const _Bool is_in = unit[0] == 'i' && unit[1] == 'n';
  int value;
  if (read_int(e->value, &value) != unit) return 0;
  if (is_cm) {
    return 150 <= value && value <= 193;
  } else if (is_in) {
    return 59 <= value && value <= 76;
  } else {
    return 0;
  }
}

static _Bool check_hair(struct entry* e) {
  if (e->value_length != 7) return 0;
  if (e->value[0] != '#') return 0;
  for (int i = 1; i < 7; i++) {
    if (!is_hex(e->value[i])) return 0;
  }
  return 1;
}

static _Bool check_eyes(struct entry* e) {
  if (e->value_length != 3) return 0;
  switch (KEY(e->value)) {
    case KEY3('a', 'm', 'b'):
    case KEY3('b', 'l', 'u'):
    case KEY3('b', 'r', 'n'):
    case KEY3('g', 'r', 'y'):
    case KEY3('g', 'r', 'n'):
    case KEY3('h', 'z', 'l'):
    case KEY3('o', 't', 'h'):
      return 1;
    default:
      return 0;
  }
}

static _Bool check_pid(struct entry* e) {
  if (e->value_length != 9) return 0;
  int x;
  return read_int(e->value, &x) == e->value + 9;
}

static _Bool part2_valid(struct entry* e) {
  switch (e->key) {
    case byr: return check_year(e, 1920, 2002);
    case iyr: return check_year(e, 2010, 2020);
    case eyr: return check_year(e, 2020, 2030);
    case hgt: return check_height(e);
    case hcl: return check_hair(e);
    case ecl: return check_eyes(e);
    case pid: return check_pid(e);
    case cid: return 0;
    default: return 0;
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
