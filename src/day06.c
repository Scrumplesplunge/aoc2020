// Exit the program with a given error message.
__attribute__((noreturn)) static void die(const char* message) {
  write(STDERR_FILENO, message, strlen(message));
  write(STDERR_FILENO, "\n", 1);
  exit(1);
}

static _Bool is_lower(char c) {
  return 'a' <= c && c <= 'z';
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

// Returns the number of bits set in an unsigned int.
unsigned bits_set(unsigned x) {
  int total = 0;
  for (int i = 0; i < 32; i++) total += (x >> i) & 1;
  return total;
  // This approach is faster, but makes the binary bigger.
  //x = (x & 0x55555555) + ((x >> 1) & 0x55555555);
  //x = (x & 0x33333333) + ((x >> 2) & 0x33333333);
  //x = (x & 0x0F0F0F0F) + ((x >> 4) & 0x0F0F0F0F);
  //x = (x & 0x00FF00FF) + ((x >> 8) & 0x00FF00FF);
  //x = (x & 0x0000FFFF) + ((x >> 16) & 0x0000FFFF);
  //return x;
}

static char input[32768];

int main() {
  const int len = read(STDIN_FILENO, input, sizeof(input));
  if (len <= 0) die("read");
  if (input[len - 1] != '\n') die("newline");
  char* i = input;
  char* const end = input + len;
  int any_count = 0, all_count = 0;
  while (i < end) {
    // Mask of answers seen in the group. Bit i is set if ('a' + i) is present.
    unsigned any = 0, all = 0x03FFFFFF;
    // Iterate over a group of passengers.
    while (i < end) {
      if (*i == '\n') {
        i++;
        break;
      }
      // Iterate over a single passenger's answers.
      unsigned mask = 0;
      while (*i != '\n') {
        if (!is_lower(*i)) die("bad");
        mask |= 1 << (*i - 'a');
        i++;
      }
      any |= mask;
      all &= mask;
      i++;
    }
    any_count += bits_set(any);
    all_count += bits_set(all);
  }
  print_int(any_count);
  print_int(all_count);
}