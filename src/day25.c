// Input: 7^x mod 20201227 and 7^y mod 20201227 for unknown x and y.
// Part 1: Find 7^xy mod 20201227.
//
// Approach: We can use the modular log operation to compute y and then use the
// identity (7^x)^y = 7^xy to find the desired result.
//
// 7^y * 7^((20201227 - 1 - k) * j) = 7^i
// 7^y = 7^(k * j + i)
//
// The naive approach to compute mod_log would be to iterate over all possible
// values for y until we find one which matches the input. However, we can do
// better by observing that 7^(20201227 - 1) mod 20201227 = 1. Taking
// k=sqrt(20201227), we can decompose y as y = k * j + i for some j and i. Then,
// mod 20201227:
//
// 7^y = 7^(k * j + i) = 7^kj * 7^i
// 7^y * 7^((20201227 - 1 - k) * j) = 7^((20201227 - 1 - k) * j) * 7^kj * 7^i
//                                  = 7^((20201227 - 1 - k + k) * j) * 7^i
//                                  = 1^j * 7^i
//                                  = 7^i
//
// Thus, if we can find i and j such that this equality holds, we will have
// found y. We can build a lookup table containing 7^i for all i in 0..k, and
// then for each j in 0..k we can check if 7^((20201227 - 1 - k) * j) is in the
// lookup table. The total time to build the lookup table and search through it
// is O(k log k).

#include "util/die.h"
#include "util/print_int.h"
#include "util/read_int.h"

static unsigned mod_mul(unsigned a, unsigned b) {
  unsigned result = 0;
  for (int i = 0; i < 8; i++) {
    unsigned digits = (b >> (28 - 4 * i)) % 16;
    result = (16 * result + digits * a) % 20201227;
  }
  return result;
}

static unsigned mod_exp(unsigned a, unsigned b) {
  unsigned result = 1;
  for (int i = 0; i < 32; i++) {
    const bool bit = (b >> (31 - i)) % 2;
    result = mod_mul(result, result);
    if (bit) result = mod_mul(result, a);
  }
  return result;
}

// An entry stores a pair: 7^exponent = value (mod 20201227)
struct entry {
  unsigned value;
  unsigned exponent;
};

static void sift_down(struct entry* values, int num_values, int i) {
  const struct entry x = values[i];
  while (true) {
    const int l = 2 * i + 1, r = l + 1;
    if (num_values <= l) break;
    const int max_child =
        r < num_values && values[l].value < values[r].value ? r : l;
    if (x.value >= values[max_child].value) break;
    values[i] = values[max_child];
    i = max_child;
  }
  values[i] = x;
}

// Find x such that a^x = b (mod 20201227).
static unsigned mod_log(unsigned a, unsigned b) {
  enum { table_size = 5000 };
  struct entry table[table_size];
  // Populate the table values.
  unsigned e = 1;
  for (int i = 0; i < table_size; i++) {
    table[i] = (struct entry){.value = e, .exponent = i};
    e = a * e % 20201227;
  }
  // Sort the table.
  for (int i = table_size - 1; i >= 0; i--) sift_down(table, table_size, i);
  for (int i = table_size - 1; i >= 0; i--) {
    const struct entry x = table[0];
    table[0] = table[i];
    sift_down(table, i, 0);
    table[i] = x;
  }
  // Search for the exponent.
  const unsigned factor = mod_exp(a, 20201227 - 1 - table_size);
  unsigned exponent = 0;
  e = b;
  while (true) {
    // Find the value in the table.
    unsigned i = 0, j = table_size;
    while (j - i > 1) {
      unsigned mid = i + (j - i) / 2;
      if (table[mid].value <= e) {
        i = mid;
      } else {
        j = mid;
      }
    }
    if (table[i].value == e) return exponent + table[i].exponent;
    e = mod_mul(e, factor);
    exponent += table_size;
  }
}

int main() {
  char buffer[32];
  const int length = read(STDIN_FILENO, buffer, sizeof(buffer));
  if (length <= 0) die("read");
  if (buffer[length - 1] != '\n') die("newline");
  unsigned door, card;
  const char* i = buffer;
  i = read_int(i, &door);
  if (*i != '\n') die("line");
  i = read_int(i + 1, &card);
  if (*i != '\n') die("line");
  if (i + 1 - buffer != length) die("syntax");
  // Find the loop size for the door.
  const unsigned loop_size = mod_log(7, door);
  // Compute the key.
  print_int(mod_exp(card, loop_size));
}
