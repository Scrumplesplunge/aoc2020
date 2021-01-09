// Input: a sequence of instructions consisting of a direction and a magnitude.
// N, E, S, and W are compass directions, while F, L and R are forward, left,
// and right respectively. For L and R, the magnitude is an angle in degrees
// that is always a multiple of 90.
// Part 1: Move the ship according to the direction and magnitudes provided and
// find the manhattan distance of the boat from its starting point.
// Part 2: Instructions N, E, S, W, L, and R actually affect a waypoint relative
// to the boat, while Fn means to move the boat by n multiples of the waypoint
// offset. After applying the newly interpreted instructions, find the manhattan
// distance of the boat from its starting point.
//
// Approach: the solution is fairly straightforward. We will use a vector to
// represent directions instead of an enum or an angle. This makes it trivial to
// move in the right direction and generalises well to part 2. One trick we can
// apply is to use aliasing pointers to give three names (position, direction,
// instruction_target) to 2 variables (boat position, offset). By varying the
// assignment of these aliases we can solve both parts of the problem with the
// same code.

#include "util/die.h"
#include "util/print_int.h"
#include "util/read_int16.h"

enum action {
  north,
  south,
  east,
  west,
  left,
  right,
  forward,
};

struct instruction {
  enum action action;
  unsigned short value;
};

enum { max_instructions = 1024 };
static struct instruction instructions[max_instructions];
static int num_instructions;

static enum action parse_action(char c) {
  switch (c) {
    case 'N': return north;
    case 'S': return south;
    case 'E': return east;
    case 'W': return west;
    case 'L': return left;
    case 'R': return right;
    case 'F': return forward;
    default: die("bad action");
  }
}

static void read_input() {
  char temp[8192];
  const int length = read(STDIN_FILENO, temp, sizeof(temp));
  if (length <= 0) die("read");
  if (temp[length - 1] != '\n') die("newline");
  const char* i = temp;
  const char* const end = temp + length;
  while (i != end) {
    if (num_instructions == max_instructions) die("too many");
    struct instruction* instruction = &instructions[num_instructions++];
    instruction->action = parse_action(*i);
    unsigned short value;
    i = read_int16(i + 1, &value);
    if (instruction->action == left || instruction->action == right) {
      // We'll go off the grid if the angles aren't multiples of 90, and it is
      // easier to work with the quotient anyway, so we'll assume that the
      // angles are all 90, 180, or 270 and then divide them by 90.
      // if (value == 0 || value >= 360 || value % 90 != 0) die("angle");
      value /= 90;
    }
    instruction->value = value;
    if (*i != '\n') die("line");
    i++;
  }
}

struct vec { int x, y; };

static struct vec rotate_left(unsigned char amount, struct vec v) {
  switch (amount) {
    case 1: return (struct vec){v.y, -v.x};
    case 2: return (struct vec){-v.x, -v.y};
    case 3: return (struct vec){-v.y, v.x};
  }
  die("bug");
}

static struct vec rotate_right(unsigned char amount, struct vec v) {
  switch (amount) {
    case 1: return (struct vec){-v.y, v.x};
    case 2: return (struct vec){-v.x, -v.y};
    case 3: return (struct vec){v.y, -v.x};
  }
  die("bug");
}

// Apply the instruction set with the given position and direction vectors. The
// NESW instructions will affect the instruction_target, which should alias
// either position or direction.
static int travel(struct vec* position, struct vec* direction,
                  struct vec* instruction_target) {
  for (int i = 0; i < num_instructions; i++) {
    switch (instructions[i].action) {
      case north:
        instruction_target->y -= instructions[i].value;
        break;
      case south:
        instruction_target->y += instructions[i].value;
        break;
      case east:
        instruction_target->x += instructions[i].value;
        break;
      case west:
        instruction_target->x -= instructions[i].value;
        break;
      case forward:
        position->x += direction->x * instructions[i].value;
        position->y += direction->y * instructions[i].value;
        break;
      case left:
        *direction = rotate_left(instructions[i].value, *direction);
        break;
      case right:
        *direction = rotate_right(instructions[i].value, *direction);
        break;
    }
  }
  const int x = position->x < 0 ? -position->x : position->x;
  const int y = position->y < 0 ? -position->y : position->y;
  return x + y;
}

static int part1() {
  struct vec position = {0, 0}, direction = {1, 0};
  return travel(&position, &direction, &position);
}

static int part2() {
  struct vec position = {0, 0}, waypoint = {10, -1};
  return travel(&position, &waypoint, &waypoint);
}

int main() {
  read_input();
  print_int(part1());
  print_int(part2());
}
