// Input: a list of luggage rules. Each rule is of the form:
//   <style> bag[s] contain [<count> <style> bag[s], ... | no other bags].
// Part 1: How many styles of bag can recursively contain a shiny gold bag?
// Part 2: How many bags must be contained inside a shiny gold bag?
//
// Approach: we will use string interning to transform style names into
// small consecutive integers that can be used as array indices. Using this as
// a building block, we will parse all rules into two arrays of linked lists.
// parents[style] and children[style] are the lists of styles that can be
// a direct parent or child of the given style of bag, respectively.
//
// To solve part 1, we will perform a depth-first search of all parents of shiny
// gold bags. To avoid duplicate work, we have a bitset of styles that have been
// seen.
//
// To solve part 2, we perform a depth-first search of children and compute the
// total transitive number of children that a given style of bag must have.
// Again, to avoid duplicate work, we have a bitset to tell us which bags have
// already been processed, and we additionally cache the count for each bag type
// so that we can reuse it for subsequent calculations.

#include "util/die.h"
#include "util/memset.h"
#include "util/print_int.h"
#include "util/read_int.h"
#include "util/strcmp.h"

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
  if (after == NULL) die("syntax");
  return after;
}

struct style {
  const char* name;
  struct style* next;
};

enum { max_styles = 1024 };
static struct style styles[max_styles];
static int num_styles;
static struct style* style_map[256];

// Return the unique ID for a given style name.
static int intern_style(const char* name) {
  const unsigned char index = ((unsigned)name[0]) * 17 + name[1];
  for (struct style* i = style_map[index]; i != NULL; i = i->next) {
    if (strcmp(i->name, name) == 0) return i - styles;
  }
  if (num_styles == max_styles) die("styles");
  // Style is new: allocate new id.
  const int id = num_styles;
  num_styles++;
  styles[id].name = name;
  styles[id].next = style_map[index];
  style_map[index] = &styles[id];
  return id;
}

// Parse a style name.
static char* read_style(char* i, int* style) {
  const char* temp = i;
  while (*i != ' ') i++;
  i++;
  while (*i != ' ') i++;
  *i = 0;
  i++;
  *style = intern_style(temp);
  return i;
}

static char buffer[65536];

struct node {
  short style;
  short count;
  struct node* next;
};

enum { max_nodes = 4096 };
static struct node nodes[max_nodes];
static int num_nodes;
static struct node* parents[max_styles];
static struct node* children[max_styles];

// Part 1: Visit all bags that can be transitive parents of a certain bag.
static bool visited[max_styles];
static void visit(int root) {
  int stack[max_nodes] = {root};
  int stack_size = 1;
  while (stack_size) {
    int x = stack[--stack_size];
    for (struct node* i = parents[x]; i != NULL; i = i->next) {
      visited[i->style] = true;
      if (stack_size == max_nodes) die("overflow");
      stack[stack_size++] = i->style;
    }
  }
}

// Part 2: Count the number of bags that must be contained by a certain bag.
struct count {
  unsigned counted : 1;
  unsigned value : 31;
};
static struct count counts[max_styles];
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
    while (true) {
      unsigned count;
      char* after = try_consume(i, "no other bags");
      if (after) {
        i = after;
        break;
      }
      i = (char*)read_int(i, &count);
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
