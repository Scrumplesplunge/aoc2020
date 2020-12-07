// 30112 too low

// Exit the program with a given error message.
__attribute__((noreturn)) static void die(const char* message) {
  write(STDERR_FILENO, message, strlen(message));
  write(STDERR_FILENO, "\n", 1);
  exit(1);
}

// Print an integer in decimal, followed by a newline.
static void print_int(unsigned x) {
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

static _Bool is_digit(char c) {
  return '0' <= c && c <= '9';
}

static _Bool is_lower(char c) {
  return 'a' <= c && c <= 'z';
}

// Consume a prefix from a string (returning the position after the prefix), or
// return NULL on failure.
static char* try_consume(char* input, const char* prefix) {
  while (*prefix) {
    if (*input - *prefix) return NULL;
    input++, prefix++;
  }
  return input;
}

// Like try_consume, but die on match failure.
static char* consume(char* input, const char* prefix) {
  char* after = try_consume(input, prefix);
  if (input == NULL) die("syntax");
  return after;
}

// Read a decimal integer from the string at input into value, returning the
// address of the first byte after the integer.
static char* read_int(char* input, int* value) {
  if (!is_digit(*input)) die("int");
  int temp = 0;
  while (is_digit(*input)) {
    temp = 10 * temp + (*input - '0');
    input++;
  }
  *value = temp;
  return input;
}

enum { max_styles = 1024 };
const char* styles[max_styles];
int num_styles;
char* styles_end;

// Return the unique ID for a given style name.
static int intern_style(const char* style) {
  for (int i = 0; i < num_styles; i++) {
    if (strcmp(styles[i], style) == 0) return i;
  }
  if (num_styles == max_styles) die("styles");
  // Style is new: allocate new id.
  const int id = num_styles;
  num_styles++;
  styles[id] = style;
  return id;
}

// Parse a style name.
static char* read_style(char* i, int* style) {
  const char* temp = i;
  while (is_lower(*i)) i++;
  i++;
  while (is_lower(*i)) i++;
  *i = 0;
  i++;
  *style = intern_style(temp);
  return i;
}

char buffer[65536];

struct node {
  int style;
  int count;
  struct node* next;
};

enum { max_nodes = 4096 };
struct node nodes[max_nodes];
int num_nodes;
struct node* parents[max_styles];
struct node* children[max_styles];

// Part 1: Visit all bags that can be transitive parents of a certain bag.
_Bool visited[max_styles];
static void visit(int root) {
  for (struct node* i = parents[root]; i != NULL; i = i->next) {
    visited[i->style] = 1;
    visit(i->style);
  }
}

// Part 2: Count the number of bags that must be contained by a certain bag.
struct count {
  unsigned counted : 1;
  unsigned value : 31;
};
struct count counts[max_styles];
static unsigned count_children(int root) {
  if (counts[root].counted) return counts[root].value;
  unsigned total = 0;
  for (struct node* i = children[root]; i != NULL; i = i->next) {
    total += i->count * (1 + count_children(i->style));
  }
  counts[root].counted = 1;
  counts[root].value = total;
  return total;
}

int main() {
  const int len = read(STDIN_FILENO, buffer, sizeof(buffer));
  if (len <= 0) die("read");
  if (buffer[len - 1] != '\n') die("newline");
  char* i = buffer;
  char* const end = buffer + len;
  while (i != end) {
    // Read a style name.
    int style;
    i = read_style(i, &style);
    i = consume(i, "bags contain ");
    while (1) {
      int count;
      char* after = try_consume(i, "no other bags");
      if (after) {
        i = after;
        break;
      }
      i = read_int(i, &count);
      i = consume(i, " ");
      int inner_style;
      i = read_style(i, &inner_style);
      i = consume(i, count == 1 ? "bag" : "bags");
      if (num_nodes == max_nodes) die("too many");
      struct node* parent = &nodes[num_nodes++];
      parent->style = style;
      parent->count = count;
      parent->next = parents[inner_style];
      parents[inner_style] = parent;
      struct node* child = &nodes[num_nodes++];
      child->style = inner_style;
      child->count = count;
      child->next = children[style];
      children[style] = child;
      if (*i != ',') break;
      i = consume(i, ", ");
    }
    if (*i != '.') die("end");
    i += 2;
  }
  // Transitively discover all bags which can hold shiny gold bags.
  const int shiny_gold = intern_style("shiny gold");
  visit(shiny_gold);
  int total = 0;
  for (int i = 0; i < max_styles; i++) total += visited[i];
  print_int(total);
  print_int(count_children(shiny_gold));
}
