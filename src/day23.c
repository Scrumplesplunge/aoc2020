// Exit the program with a given error message.
__attribute__((noreturn)) static void die(const char* message) {
  write(STDERR_FILENO, message, strlen(message));
  write(STDERR_FILENO, "\n", 1);
  exit(1);
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
  // struct node* j = first;
  // for (int i = 0; i < 9; i++, j = j->next) {
  //   char out[] = "  ? ";
  //   out[2] = j - nodes + '1';
  //   if (j == current) {
  //     out[1] = '(';
  //     out[3] = ')';
  //   }
  //   write(STDOUT_FILENO, out, 4);
  // }
  // write(STDOUT_FILENO, "\n", 1);
}

int main() {
  char buffer[10];
  const int length = read(STDIN_FILENO, buffer, sizeof(buffer));
  if (length != 10) die("read");
  if (buffer[9] != '\n') die("newline");
  struct node nodes[9];
  struct node* first;
  struct node** previous = &first;
  for (int i = 0; i < 9; i++) {
    const int value = buffer[i] - '0';
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
