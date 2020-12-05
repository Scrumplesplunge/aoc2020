// Exit the program with a given error message.
__attribute__((noreturn)) static void die(const char* message) {
  write(STDERR_FILENO, message, strlen(message));
  write(STDERR_FILENO, "\n", 1);
  exit(1);
}

// Print an integer in decimal, followed by a newline.
static void print_int(int x) {
  char buffer[16];
  buffer[15] = '\n';
  int i = 15;
  do {
    --i;
    buffer[i] = '0' + (x % 10);
    x /= 10;
  } while (x);
  write(STDOUT_FILENO, buffer + i, 16 - i);
}

// Bitmask for seats. seats[row] is a full row with bit i set if column i is
// populated by some boarding pass.
unsigned char seats[128] = {0};

int main() {
  int max_id = 0;
  while (1) {
    char code[11];
    const int len = read(STDIN_FILENO, code, 11);
    if (len == 0) break;
    if (len != 11) die("read");
    if (code[10] != '\n') die("newline");
    int row = 0, column = 0;
    for (int i = 0; i < 7; i++) row = (row << 1) | (code[i] == 'B');
    for (int i = 7; i < 10; i++) column = (column << 1) | (code[i] == 'R');
    const int id = row << 3 | column;
    if (id > max_id) max_id = id;
    seats[row] |= 1 << column;
  }
  // Part 1: print the maximum boarding pass ID.
  print_int(max_id);
  // Part 2: find the id of the unpopulated seat. Some seats at the front and
  // back do not exist, so we need to disregard those.
  int start = 0;
  while (seats[start] != 255) start++;
  int end = 127;
  while (seats[end] != 255) end--;
  for (int row = start; row < end; row++) {
    if (seats[row] == 255) continue;
    for (int column = 0; column < 8; column++) {
      if (~seats[row] & (1 << column)) {
        print_int(row << 3 | column);
        return 0;
      }
    }
  }
  die("not found");
}
