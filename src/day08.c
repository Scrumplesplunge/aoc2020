// Input: a small assembly code program with the following opcodes:
//   nop x - Do nothing.
//   acc x - Adjust the accumulator by the given amount.
//   jmp x - Jump to a new instruction relative to this one.
// The assembly program is guaranteed to have an infinite loop.
// Part 1: Find the value of the accumulator immediately before the earliest
// point when an instruction is executed for a second time.
// Part 2: Find the sole `nop` or `jmp` instruction that can be switched to
// `jmp` or `nop` respectively such that the program will terminate, and thus
// find the value of the accumulator at the point when the program terminates.
//
// Approach: parse the program into an array of structs representing each
// instruction. We will reserve space for tracking state about each instruction.
//
// For part 1, we can simply execute the program, setting `op.seen` for each
// instruction that we execute and terminating when we see an op that has
// already been seen.
//
// For part 2, we know that we can only change a single nop to a jmp, or
// a single jmp to a nop. Firsly, it only makes sense to change an instruction
// which is already reachable from the starting point, as other instruction
// changes would have no effect. Secondly, if we are to change a nop to a jmp,
// it must mean that resuming from the destination of that jmp is will
// eventually terminate, and if we are to change a jmp to a nop, it must mean
// that resuming from the next instruction after this one will eventually
// terminate. Thus, if we can determine whether execution from a given point
// will eventually terminate, then we can try each reachable nop or jmp in turn
// and find the one which will result in termination.
//
// Execution starting at a given instruction will terminate if:
//   * The instruction is a nop or acc and is the last instruction in the
//     program.
//   * The instruction is a jmp which jumps beyond the last instruction in the
//     program.
//   * Execution would terminate starting at the next instruction to execute
//     after this one.
//
// We can recursively compute this for each starting position, memoizing the
// values that we find in the process. The memoization ensures that we will
// compute the value for a given starting position only once, so the total
// processing time is bounded by the number of instructions in the input.

#include "util/die.h"
#include "util/print_int.h"
#include "util/read_int.h"
#include "util/is_lower.h"

enum opcode {
  nop,
  acc,
  jmp,
};

struct operation {
  // True if this instruction can be reached from the start of the program.
  unsigned reachable : 1;
  // True if we have already examined this node for termination.
  unsigned seen : 1;
  // True if we can reach the end from this instruction.
  unsigned terminates : 1;
  unsigned opcode : 2;
  int argument : 27;
};

enum { max_code_size = 1 << 20 };
static struct operation code[max_code_size];
static int code_size;

#define C(x) ((unsigned)(unsigned char)(x))
#define KEY3(a, b, c) (C(a) | C(b) << 8 | C(c) << 16)

static char buffer[8 << 20];
static void read_input() {
  int len = read(STDIN_FILENO, buffer, sizeof(buffer));
  if (len <= 0) die("read");
  if (buffer[len - 1] != '\n') die("newline");
  char* i = buffer;
  char* const end = buffer + len;
  while (i != end) {
    if (code_size == max_code_size) die("too long");
    struct operation* op = &code[code_size++];
    if (!is_lower(i[0]) || !is_lower(i[1]) || !is_lower(i[2]) || i[3] != ' ') {
      die("syntax");
    }
    switch (KEY3(i[0], i[1], i[2])) {
      case KEY3('n', 'o', 'p'):
        op->opcode = nop;
        break;
      case KEY3('a', 'c', 'c'):
        op->opcode = acc;
        break;
      case KEY3('j', 'm', 'p'):
        op->opcode = jmp;
        break;
      default:
        die("bad op");
    }
    i += 4;
    const bool negative = *i == '-';
    i++;
    unsigned value;
    i = (char*)read_int(i, &value);
    op->argument = negative ? -value : value;
    if (*i != '\n') die("line");
    i++;
  }
}

static bool run(int* result) {
  for (int i = 0; i < code_size; i++) code[i].reachable = 0;
  int i = 0;
  int accumulator = 0;
  while (i < code_size) {
    struct operation* op = &code[i];
    if (op->reachable) {
      *result = accumulator;
      return false;
    }
    op->reachable = 1;
    switch (op->opcode) {
      case nop:
        i++;
        break;
      case acc:
        accumulator += op->argument;
        i++;
        break;
      case jmp:
        i += op->argument;
        break;
    }
  }
  *result = accumulator;
  return true;
}

static int part1() {
  int result;
  if (run(&result)) die("part1 terminates");
  return result;
}

static int stack[max_code_size];
static bool terminates(int root) {
  if (code[root].seen) return code[root].terminates;
  stack[0] = root;
  int size = 1;
  while (size) {
    const int address = stack[--size];
    struct operation* op = &code[address];
    const int offset = op->opcode == jmp ? op->argument : 1;
    op->seen = true;
    if (address + offset >= code_size) {
      op->terminates = true;
    } else if (op[offset].seen) {
      op->terminates = op[offset].terminates;
    } else {
      stack[size++] = address;
      stack[size++] = address + offset;
    }
  }
  return code[root].terminates;
}

static int part2() {
  for (int i = 0; i < code_size; i++) {
    struct operation* op = &code[i];
    // It's only worth adjusting instructions which are initially reachable.
    if (!op->reachable) continue;
    switch (op->opcode) {
      case nop:
        if (terminates(i + op->argument)) {
          op->opcode = jmp;
          int result;
          if (!run(&result)) die("bug");
          return result;
        }
        break;
      case jmp:
        if (terminates(i + 1)) {
          op->opcode = nop;
          int result;
          if (!run(&result)) die("bug");
          return result;
        }
        break;
      case acc:
        break;
    }
  }
  die("no change works");
}

int main() {
  read_input();
  print_int(part1());
  print_int(part2());
}
