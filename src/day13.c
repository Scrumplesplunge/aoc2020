// Input: A number representing your earliest possible departure time, and
// a list of bus IDs that are in service with an x where a bus is not running.
// A bus with ID x arrives at the seaport every x minutes.
// Part 1: What is the earliest bus that you could board, multiplied by the
// amount of time you will have to wait for that bus?
// Part 2: The index of a bus is the position it appears in the input list. Find
// a time t such that for all buses, the bus with index i departs at time t+i.
//
// Approach: part 1 is fairly straightforward. For each bus, find the soonest
// departure after your earliest possible departure time. Pick the smallest one
// across all buses. We can solve part 2 inductively by maintaining two
// variables:
//   * earliest - The earliest t which works for all buses with index < i.
//   * period - The amount of time between consecutive instances of these buses
//     being satisfied.
// These values are initially i=0, earliest=0, period=1 (meaning that when no
// buses need to be constrained, the earliest match is t=0 and every consecutive
// minute also suffices). We then iterate over increasing values for i to update
// the values. We can consider earliest' = earliest + k * period by brute force
// over increasing k, and we can then update the period to be the least common
// multiple of the bus ids seen so far (i.e. lcm(period, bus)). This converges
// very quickly and avoids having to compute modular inverses, which is more
// complicated.

#include "util/die.h"
#include "util/division.h"
#include "util/print_int64.h"
#include "util/read_int.h"

// Round x up to the next multiple of k.
static unsigned round_up(unsigned x, unsigned k) {
  return (x + k - 1) / k * k;
}

static int part1(const char* input) {
  unsigned earliest_departure;
  enum { max_buses = 16 };
  unsigned buses[max_buses];
  int num_buses = 0;
  input = read_int(input, &earliest_departure);
  if (*input != '\n') die("bad input");
  do {
    input++;
    if (*input == 'x') {
      input++;
    } else {
      if (num_buses == max_buses) die("too large");
      input = read_int(input, &buses[num_buses++]);
    }
  } while (*input == ',');
  if (*input != '\n') die("bad input");
  unsigned bus = 0, time = round_up(earliest_departure, buses[0]);
  for (int i = 1; i < num_buses; i++) {
    const unsigned new_time = round_up(earliest_departure, buses[i]);
    if (new_time < time) {
      bus = i;
      time = new_time;
    }
  }
  return buses[bus] * (time - earliest_departure);
}

static unsigned long long gcd(unsigned long long a, unsigned long long b) {
  while (b != 0) {
    const unsigned long long temp = b;
    b = a % b;
    a = temp;
  }
  return a;
}

static unsigned long long lcm(unsigned long long a, unsigned long long b) {
  return a / gcd(a, b) * b;
}

static unsigned long long part2(const char* input) {
  unsigned earliest_departure;
  enum { max_buses = 128 };
  unsigned buses[max_buses];
  int num_buses = 0;
  input = read_int(input, &earliest_departure);
  if (*input != '\n') die("bad input");
  do {
    input++;
    if (num_buses == max_buses) die("too large");
    if (*input == 'x') {
      input++;
      buses[num_buses++] = 0;
    } else {
      input = read_int(input, &buses[num_buses++]);
    }
  } while (*input == ',');
  if (*input != '\n') die("bad input");
  unsigned long long earliest = 0;
  unsigned long long period = 1;
  for (int i = 0; i < num_buses; i++) {
    if (buses[i] == 0) continue;
    // Invariant: `earliest` is the earliest time when the first i buses arrive
    // at the right minute. `period` is the amount of time until that happens
    // again.
    while ((earliest + i) % buses[i] != 0) earliest += period;
    period = lcm(period, buses[i]);
  }
  return earliest;
}

static char buffer[256];
int main() {
  int length = read(STDIN_FILENO, buffer, sizeof(buffer) - 1);
  if (length <= 0) die("read");
  if (buffer[length - 1] != '\n') die("newline");
  buffer[length] = '\0';
  print_int64(part1(buffer));
  print_int64(part2(buffer));
}
