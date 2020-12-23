// Exit the program with a given error message.
__attribute__((noreturn)) static void die(const char* message) {
  write(STDERR_FILENO, message, strlen(message));
  write(STDERR_FILENO, "\n", 1);
  exit(1);
}

struct node {
  unsigned char value;
  struct node* next;
};

int main() {
  char buffer[10];
  const int length = read(STDIN_FILENO, buffer, sizeof(buffer));
  if (length != 10) die("read");
  if (buffer[9] != '\n') die("newline");
  struct node nodes[9];
  for (int i = 0; i < 9; i++) {
    nodes[i].value = buffer[i] - '0';
    nodes[i].next = &nodes[i + 1];
  }
  nodes[8].next = &nodes[0];
  struct node* current = &nodes[0];
  for (int move = 0; move < 100; move++) {
    // Remove 3 nodes from after the current one.
    struct node* const removed = current->next;
    current->next = current->next->next->next->next;
    struct node* destination = NULL;
    int best_diff = 10;
    for (struct node* i = current->next; i != current; i = i->next) {
      int diff = (current->value - i->value + 10) % 10;
      if (diff < best_diff) {
        best_diff = diff;
        destination = i;
      }
    }
    removed->next->next->next = destination->next;
    destination->next = removed;
    current = current->next;
  }
  // Rotate until 1 is at the start.
  while (current->value != 1) current = current->next;
  char out[9];
  struct node* j = current->next;
  for (int i = 0; i < 8; i++, j = j->next) {
    out[i] = j->value + '0';
  }
  out[8] = '\n';
  write(STDOUT_FILENO, out, 9);
}
