// Input: A sequence of instructions which are one of:
//   * `mask = <value>` - Set the current mask value.
//   * `mem[<address>] = <value>` - Assign a value to memory.
// Each character of the mask is either '0', '1', or 'X'. 'X' is a wildcard.
// Part 1: The mask is applied to each value before it is written to memory.
// Wildcard bits remain unchanged, but other bits are forced to match the mask.
// Find the sum of all values left in memory at the end of the program.
// Part 2: The mask is applied to memory addresses before assigning a value. For
// each bit, the following transformation is applied:
//   * If the mask value is 0, the corresponding address bit is unchanged.
//   * If the mask value is 1, the corresponding address bit is set to 1.
//   * If the mask value is X, the corresponding address bit is both 0 *and* 1.
// An assignment updates all addresses represented by the post-mask value. Find
// the sum of all values left in memory at the end of the program.
//
// Approach: we can parse the program into an array of instructions that is
// suitable for both parts of the question. To parse the mask, we will use two
// separate bitmasks: a "set" mask (which is 1 for each 1 in the mask), and
// a "clear" mask (which is 1 for each 0 in the mask).
//
// For part 1, we can apply the mask with some bitwise operations:
//   x' = (x | set_mask) & ~clear_mask;
//
// For part 2, we need to iterate over many addresses represented by the masking
// set. There's a nifty trick we can use to iterate over exactly the right
// addresses:
//   // Constant with 1 for each X in the mask.
//   floating_mask = 0xFFFFFFFFFULL & ~set_mask & ~clear_mask;
//   // Iterate this until it gets back to 0.
//   floating := (floating + 1 + ~floating_mask) & floating_mask;
// This works by setting all non-floating digits to 1, causing any increment to
// carry into the next floating digit and thus allowing us to act as if all the
// Xs were a simple binary number. By iterating over values for floating and
// XORing it with the address, we will iterate over all matching addresses.
//
// We can then simply run the program, applying all assignments, and calculate
// the resulting sum. Since the addresses can be large, we use a hashtable
// instead of an array. However, since the maximum number of Xs in a mask is
// small, we don't assign to many addresses in total and the size remains
// manageable.

#include "util/die.h"
#include "util/print_int64.h"
#include "util/read_int.h"

enum operation {
  mask,
  assign,
};

struct instruction {
  enum operation operation;
  unsigned long long a, b;
};

enum { max_instructions = 1024, memory_size = 65536 };
static struct instruction instructions[max_instructions];
static int num_instructions;

static void read_input() {
  char buffer[65536];
  const int length = read(STDIN_FILENO, buffer, sizeof(buffer));
  if (length <= 0) die("read");
  if (buffer[length - 1] != '\n') die("newline");
  const char* i = buffer;
  const char* const end = buffer + length;
  while (i != end) {
    if (num_instructions == max_instructions) die("too many");
    if (i[0] != 'm') die("bad");
    if (i[1] == 'a') {
      // mask = value
      i += 7;
      unsigned long long set_mask = 0, clear_mask = 0;
      while (*i != '\n') {
        set_mask = set_mask << 1 | (*i == '1');
        clear_mask = clear_mask << 1 | (*i == '0');
        i++;
      }
      instructions[num_instructions++] = (struct instruction){
        .operation = mask,
        .a = set_mask,
        .b = clear_mask,
      };
    } else if (i[1] == 'e') {
      // mem[address] = value
      unsigned address, value;
      i = read_int(i + 4, &address);
      if (address >= memory_size) die("address");
      if (*i != ']') die("bad");
      i = read_int(i + 4, &value);
      instructions[num_instructions++] = (struct instruction){
        .operation = assign,
        .a = address,
        .b = value,
      };
    } else {
      die("bad");
    }
    if (*i != '\n') die("line");
    i++;
  }
}

static unsigned long long memory[memory_size];
static unsigned long long part1() {
  unsigned long long set_mask = 0, clear_mask = 0;
  for (int i = 0; i < num_instructions; i++) {
    switch (instructions[i].operation) {
      case mask:
        set_mask = instructions[i].a;
        clear_mask = instructions[i].b;
        break;
      case assign:
        memory[instructions[i].a] =
            (instructions[i].b | set_mask) & ~clear_mask;
        break;
    }
  }
  unsigned long long total = 0;
  for (int i = 0; i < memory_size; i++) {
    total += memory[i];
  }
  return total;
}

struct slot {
  unsigned long long address;
  unsigned long long value;
  struct slot* next;
};
enum { max_slots = 1 << 20, slot_map_size = 1 << 18 };
static struct slot slots[max_slots];
static int num_slots;
static struct slot* slot_map[slot_map_size];

static unsigned bucket(unsigned long long address) {
  return (address ^ (address >> 18)) % slot_map_size;
}

static struct slot* get_slot(unsigned long long address) {
  struct slot** slot = &slot_map[bucket(address)];
  while (*slot && (*slot)->address != address) {
    slot = &(*slot)->next;
  }
  if (*slot) return *slot;
  if (num_slots == max_slots) die("too many");
  *slot = &slots[num_slots++];
  (*slot)->address = address;
  (*slot)->value = 0;
  return *slot;
}

static unsigned long long part2() {
  unsigned long long set_mask = 0, floating_mask = 0;
  for (int i = 0; i < num_instructions; i++) {
    switch (instructions[i].operation) {
      case mask:
        set_mask = instructions[i].a;
        floating_mask = 0xFFFFFFFFFULL & ~set_mask & ~instructions[i].b;
        break;
      case assign: {
        const unsigned long long base_address = instructions[i].a | set_mask;
        // Iterate over all the possible combinations of floating bits.
        unsigned long long floating_values = 0;
        do {
          get_slot(base_address ^ floating_values)->value = instructions[i].b;
          floating_values =
              (floating_values + 1 + ~floating_mask) & floating_mask;
        } while (floating_values);
        break;
      }
    }
  }
  unsigned long long total = 0;
  for (int i = 0; i < num_slots; i++) {
    total += slots[i].value;
  }
  return total;
}

int main() {
  read_input();
  print_int64(part1());
  print_int64(part2());
}
