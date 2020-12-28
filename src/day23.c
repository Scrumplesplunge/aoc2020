#include "util/die.h"

// Print an integer in decimal, followed by a newline.
static void print_int64(unsigned long long x) {
  char buffer[24] = {[23] = '\n'};
  // Compute the decimal format one bit at a time by doubling and carrying BCD.
  for (int i = 63; i >= 0; i--) {
    for (int j = 0; j < 23; j++) buffer[j] *= 2;
    if ((x >> i) & 1) buffer[22]++;
    char carry = 0;
    for (int j = 22; j >= 0; j--) {
      char temp = buffer[j] + carry;
      buffer[j] = temp % 10;
      carry = temp / 10;
    }
  }
  // Compute the most significant digit and truncate the output.
  int i = 0;
  while (buffer[i] == 0) i++;
  const int start = i < 22 ? i : 22;
  for (int j = start; j < 23; j++) buffer[j] += '0';
  write(STDOUT_FILENO, buffer + start, 24 - start);
}

struct node {
  struct node* next;
};

static struct node* step(
    struct node* nodes, int num_nodes, struct node* current) {
  // Remove 3 nodes from after the current one.
  struct node* const removed = current->next;
  current->next = current->next->next->next->next;
  const int a = removed - nodes, b = removed->next - nodes,
            c = removed->next->next - nodes;
  int d = current - nodes;
  do {
    d = d == 0 ? num_nodes - 1 : d - 1;
  } while (d == a || d == b || d == c);
  struct node* const destination = &nodes[d];
  removed->next->next->next = destination->next;
  destination->next = removed;
  return current->next;
}

static void part1(const char* input) {
  struct node nodes[9];
  struct node* first;
  struct node** previous = &first;
  for (int i = 0; i < 9; i++) {
    const int value = input[i] - '0';
    struct node* n = &nodes[value - 1];
    *previous = n;
    previous = &n->next;
  }
  *previous = first;
  struct node* current = first;
  for (int move = 0; move < 100; move++) current = step(nodes, 9, current);
  char out[9];
  struct node* j = nodes[0].next;
  for (int i = 0; i < 8; i++, j = j->next) {
    out[i] = j - nodes + '1';
  }
  out[8] = '\n';
  write(STDOUT_FILENO, out, 9);
}

struct node nodes[1000000];
static void part2(const char* input) {
  struct node* first;
  struct node** previous = &first;
  for (int i = 0; i < 9; i++) {
    const int value = input[i] - '0';
    struct node* n = &nodes[value - 1];
    *previous = n;
    previous = &n->next;
  }
  for (int i = 9; i < 1000000; i++) {
    *previous = &nodes[i];
    previous = &nodes[i].next;
  }
  *previous = first;
  struct node* current = first;
  for (int move = 0; move < 10000000; move++) {
    current = step(nodes, 1000000, current);
  }
  const struct node* const one = &nodes[0];
  unsigned long long a = one->next - nodes + 1;
  unsigned long long b = one->next->next - nodes + 1;
  print_int64(a * b);
}

int main() {
  char buffer[10];
  const int length = read(STDIN_FILENO, buffer, sizeof(buffer));
  if (length != 10) die("read");
  if (buffer[9] != '\n') die("newline");
  part1(buffer);
  part2(buffer);
}
