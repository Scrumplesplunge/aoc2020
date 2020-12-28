#include "util/die.h"
#include "util/printf.h"
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
enum { max_slots = 1 << 20, slot_map_size = 1 << 16 };
static struct slot slots[max_slots];
static int num_slots;
static struct slot* slot_map[slot_map_size];

static unsigned bucket(unsigned long long address) {
  unsigned bucket = 0;
  for (unsigned long long i = -1; i; i /= slot_map_size) {
    bucket ^= address % slot_map_size;
    address /= slot_map_size;
  }
  return bucket;
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
  printf("%llu\n", part1());
  printf("%llu\n", part2());
}
