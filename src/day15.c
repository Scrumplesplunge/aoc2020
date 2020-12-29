#include "util/die.h"
#include "util/memset.h"
#include "util/print_int.h"
#include "util/read_int.h"

unsigned spoken[30000001];
int main() {
  char buffer[128];
  const int length = read(STDIN_FILENO, buffer, sizeof(buffer));
  if (length <= 0) die("read");
  if (buffer[length - 1] != '\n') die("newline");
  const char* i = buffer;
  unsigned turn = 0;
  unsigned last_number;
  memset(spoken, -1, sizeof(spoken));
  while (1) {
    ++turn;
    i = read_int(i, &last_number);
    spoken[last_number] = turn;
    if (*i == '\n') break;
    if (*i != ',') die("comma");
    i++;
  }
  while (turn < 2020) {
    const unsigned answer =
        spoken[last_number] != (unsigned)-1 ? turn - spoken[last_number] : 0;
    spoken[last_number] = turn;
    turn++;
    last_number = answer;
  }
  print_int(last_number);
  while (turn < 30000000) {
    const unsigned answer =
        spoken[last_number] != (unsigned)-1 ? turn - spoken[last_number] : 0;
    spoken[last_number] = turn;
    turn++;
    last_number = answer;
  }
  print_int(last_number);
}
