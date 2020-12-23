// Exit the program with a given error message.
__attribute__((noreturn)) static void die(const char* message) {
  write(STDERR_FILENO, message, strlen(message));
  write(STDERR_FILENO, "\n", 1);
  exit(1);
}

// static unsigned long long read_input() {
//   char buffer[10];
//   const int length = read(STDIN_FILENO, buffer, sizeof(buffer));
//   if (length != 10) die("read");
//   if (buffer[9] != '\n') die("newline");
//   unsigned long long number = 0;
//   for (int i = 0; i < 9; i++) {
//     number = number << 4 | (buffer[i] - '0');
//   }
//   return number;
// }
// 
// static void print(unsigned long long x) {
//   char buffer[10];
//   for (int i = 0; i < 9; i++) {
//     buffer[8 - i] = "0123456789abcdef"[(x >> (4 * i)) % 16];
//   }
//   buffer[9] = '\n';
//   write(STDOUT_FILENO, buffer, 10);
// }

struct node {
  unsigned char value;
  unsigned char next;
};

int main() {
  char buffer[10];
  const int length = read(STDIN_FILENO, buffer, sizeof(buffer));
  if (length != 10) die("read");
  if (buffer[9] != '\n') die("newline");
  struct node nodes[9];
  for (int i = 0; i < 9; i++) {
    nodes[i].value = buffer[i] - '0';
    nodes[i].next = i + 1;
  }
  nodes[8].next = 0;
  int current = 0;
  for (int move = 0; move < 100; move++) {
    // Remove 3 nodes from after the current one.
    const int removed = nodes[current].next;
    nodes[current].next =
        nodes[nodes[nodes[nodes[current].next].next].next].next;
    int destination = -1, best_diff = 10;
    for (int i = nodes[current].next; i != current; i = nodes[i].next) {
      int diff = (nodes[current].value - nodes[i].value + 10) % 10;
      if (diff < best_diff) {
        best_diff = diff;
        destination = i;
      }
    }
    // // Find the destination value.
    // char destination = current;
    // do {
    //   destination = (destination == '1' ? '9' : destination - 1);
    //   // Skip destination cups that are among the ones in the hand.
    //   if (hand[0] != destination && hand[1] != destination &&
    //       hand[2] != destination) {
    //     break;
    //   }
    // } while (destination != current);
    // for (int i = nodes[current].next; i != current; i = nodes[i].next) {
    //   if (nodes[i].value == destination) {
    //     nodes[nodes[nodes[removed].next].next].next = nodes[i].next;
    //     nodes[i].next = removed;
    //     break;
    //   }
    // }
    nodes[nodes[nodes[removed].next].next].next = nodes[destination].next;
    nodes[destination].next = removed;
    current = nodes[current].next;
  }
  // Rotate until 1 is at the start.
  while (nodes[current].value != 1) current = nodes[current].next;
  // for (int i = 0, j = 0; i < 8; i++, j = nodes[j].next) {
  //   char out[] = "  ? ";
  //   out[2] = nodes[j].value + '0';
  //   if (j == current) {
  //     out[1] = '(';
  //     out[3] = ')';
  //   }
  //   write(STDOUT_FILENO, out, 4);
  // }
  // write(STDOUT_FILENO, "\n", 1);
  char out[9];
  for (int i = 0, j = nodes[current].next; i < 8; i++, j = nodes[j].next) {
    out[i] = nodes[j].value + '0';
  }
  out[8] = '\n';
  write(STDOUT_FILENO, out, 9);
}
