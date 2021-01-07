// Input: A sequence of rules, followed by your ticket, followed by all nearby
// tickets. Each rule is of the form:
//   <field name>: <low1>-<high1> or <low2>-<high2>
// Part 1: Compute the sum of all field values that are invalid for every rule.
// Part 2: Identify what fields correspond to what locations in the tickets and
// find the product of all six "departure ..." field values for your ticket.
//
// Approach: Part 1 is a simple linear pass. Part 2 requires matching fields to
// indices such that all rules are satisfied for all fields. To optimize this,
// we can first compute a table valid_pairs such that valid_pairs[i][f] is false
// if field f cannot possibly be index i. We can do this with a single pass over
// the tickets. Using this table to narrow down the possible candidates, we can
// search for possible field-index mappings, backtracking when we find
// a contradiction. Once we have a complete mapping, we can trivially extract
// the relevant fields from our ticket and calculate the product.

#include "util/die.h"
#include "util/memset.h"
#include "util/print_int64.h"
#include "util/read_int16.h"
#include "util/strncmp.h"

struct range {
  unsigned short min, max;
};

struct field {
  const char* name;
  struct range ranges[2];
};

enum { max_fields = 20 };
static struct field fields[max_fields];
static int num_fields;

struct ticket {
  unsigned short values[max_fields];
};

enum { max_tickets = 512 };
static struct ticket tickets[max_tickets];
static int num_tickets;

// Read a ticket into the given struct. Returns a pointer to the address
// immediately after the ticket in the input. Dies if the ticket is invalid.
static char* read_ticket(char* i, struct ticket* t) {
  int num_values = 0;
  while (true) {
    if (num_values == num_fields) die("too many values");
    i = (char*)read_int16(i, &t->values[num_values++]);
    if (*i != ',') break;
    i++;
  }
  if (*i != '\n') die("bad ticket");
  return i + 1;
}

// Returns true if the x is a valid value for field f.
static bool valid(struct field* f, int x) {
  return (f->ranges[0].min <= x && x <= f->ranges[0].max) ||
         (f->ranges[1].min <= x && x <= f->ranges[1].max);
}

// Returns true if x is an invalid value for all fields.
static bool invalid_value(int x) {
  int valid_fields = 0;
  for (int j = 0; j < num_fields; j++) {
    if (valid(&fields[j], x)) valid_fields++;
  }
  return valid_fields == 0;
}

// Returns true if t is not a valid ticket.
static bool invalid_ticket(struct ticket* t) {
  for (int i = 0; i < num_fields; i++) {
    if (invalid_value(t->values[i])) return true;
  }
  return false;
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
static bool valid_pairs[max_fields][max_fields];

// Given a partial assignment result[0..num_fields) with -1 for unassigned
// fields, and a set of taken indices where taken[i] is true if result already
// has a field assigned to i, returns the field index of the least ambiguous
// field.
static int least_ambiguous(const signed char* result, bool* taken) {
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
static bool deduce_fields(signed char* result) {
  bool taken[max_fields] = {0};
  bool all_taken = true;
  for (int f = 0; f < num_fields; f++) {
    if (result[f] != -1) {
      taken[result[f]] = true;
    } else {
      all_taken = false;
    }
  }
  if (all_taken) return true;  // All fields have been assigned, we are done.
  // Find the least ambiguous field and try each possible assignment for it.
  const int f = least_ambiguous(result, taken);
  if (f == -1) return false;
  for (int i = 0; i < num_fields; i++) {
    if (valid_pairs[i][f] && !taken[i]) {
      result[f] = i;
      if (deduce_fields(result)) return true;
    }
  }
  return false;
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
  unsigned long long total = 1;
  for (int i = 0; i < num_fields; i++) {
    if (strncmp(fields[i].name, "departure ", 10) != 0) continue;
    total *= tickets[0].values[indices[i]];
  }
  return total;
}

int main() {
  char buffer[32768];
  const int length = read(STDIN_FILENO, buffer, sizeof(buffer));
  if (length <= 0) die("read");
  if (buffer[length - 1] != '\n') die("newline");
  char* i = buffer;
  while (true) {
    if (*i == '\n') break;
    if (num_fields == max_fields) die("too many fields");
    struct field* f = &fields[num_fields++];
    f->name = i;
    while (*i != ':') i++;
    *i = '\0';
    i = (char*)read_int16(i + 2, &f->ranges[0].min);
    if (*i != '-') die("bad");
    i = (char*)read_int16(i + 1, &f->ranges[0].max);
    // " or "
    i = (char*)read_int16(i + 4, &f->ranges[1].min);
    if (*i != '-') die("bad");
    i = (char*)read_int16(i + 1, &f->ranges[1].max);
    if (*i != '\n') die("line");
    i++;
  }
  i++;
  if (strncmp(i, "your ticket:\n", 13) != 0) die("syntax");
  i += 13;
  i = read_ticket(i, &tickets[num_tickets++]);
  i++;
  if (strncmp(i, "nearby tickets:\n", 16) != 0) die("syntax");
  i += 16;
  const char* const end = buffer + length;
  while (i != end) i = read_ticket(i, &tickets[num_tickets++]);
  print_int64(part1());
  print_int64(part2());
}
