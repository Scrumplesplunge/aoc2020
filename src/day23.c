// Input: a permutation of the number 1-9.
// A game of crab cups is played using a circle of labelled cups and one cup
// identified as the current cup. In each move, the three cups clockwise of the
// current cup are removed and inserted immediately after the cup with the label
// equal to the current cup's label minus one. The new current cup is then the
// next cup clockwise of the current cup.
// Part 1: Using the 9 input cups as the circle, find the sequence of cups
// following cup 1 after 100 moves.
// Part 2: Using the 9 input cups, followed by all of the cups 10-1000000 in
// order, find the two cups following cup 1 after 10000000 moves and return the
// product of their labels.
//
// Approach: We'll use a circular linked list to represent the circle of cups.
// This makes the insertions and deletions efficient. To make the destination
// lookup efficient, the linked list nodes will be stored in an array indexed by
// cup label. Since the position encodes the label, the nodes only need to store
// the pointer to the next node.

#include "util/die.h"
#include "util/print_int64.h"

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

static struct node nodes[1000000];
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
