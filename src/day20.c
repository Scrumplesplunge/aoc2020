// Exit the program with a given error message.
__attribute__((noreturn)) static void die(const char* message) {
  write(STDERR_FILENO, message, strlen(message));
  write(STDERR_FILENO, "\n", 1);
  exit(1);
}

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

static _Bool is_digit(char c) {
  return '0' <= c && c <= '9';
}

// Read a decimal integer from the string at input into value, returning the
// address of the first byte after the integer.
static const char* read_int(const char* input, unsigned short* value) {
  if (!is_digit(*input)) die("int");
  unsigned short temp = 0;
  while (is_digit(*input)) {
    temp = 10 * temp + (*input - '0');
    input++;
  }
  *value = temp;
  return input;
}

// Composable transformations:
// +-one-+ +four-+ +-eno-+ +-ruof+ +eerht+ +-owt-+ +three+ +-two-+
// |     | e     | |     | |     e f     | t     | |     f |     t
// r  0  t e  1  o t  0  r o  1  e o  0  o h  1  e o  0  o e  1  h
// u  0  w r  0  n w  1  u n  1  r u  0  w r  0  n w  1  u n  1  r
// o  0  o h  0  e o  0  o e  0  h r  1  t e  1  o t  1  r o  1  e
// f     | t     | |     f |     t |     | e     | |     | |     e
// +eerht+ +-owt-+ +three+ +-two-+ +-one-+ +four-+ +-eno-+ +-ruof+
enum transformation {
  rotate_right = 1,  // Apply a quarter turn right
  flip_horizontal = 2,  // Flip the tile horizontally
  flip_vertical = 4,  // Flip the tile vertically
};

struct edges {
  unsigned short top, right, bottom, left;
};

enum { tile_size = 10 };
struct tile {
  unsigned short id;
  unsigned short transformation;  // Stores the transformation mask, if known.
  struct edges edges[8];  // 8 possible arrangements of 4 edges.
  unsigned short cells[tile_size];
};

enum { max_tiles = 256 };
struct tile tiles[max_tiles];
int num_tiles;

// Reverse the bits in an unsigned short.
static unsigned short reverse(unsigned short x) {
  x = (x & 0xFF00) >> 8 | (x & 0x00FF) << 8;
  x = (x & 0xF0F0) >> 4 | (x & 0x0F0F) << 4;
  x = (x & 0xCCCC) >> 2 | (x & 0x3333) << 2;
  x = (x & 0xAAAA) >> 1 | (x & 0x5555) << 1;
  return x;
}

// Turn the ith column of a tile into an unsigned short.
static unsigned short column(struct tile* t, int c) {
  unsigned short value = 0;
  for (int i = 0; i < tile_size; i++) {
    value = value << 1 | ((t->cells[i] >> c) & 1);
  }
  return value;
}

static void read_input() {
  char buffer[32768];
  const int length = read(STDIN_FILENO, buffer, sizeof(buffer));
  if (length <= 0) die("read");
  if (buffer[length - 1] != '\n') die("newline");
  const char* i = buffer;
  const char* const end = buffer + length;
  while (i != end) {
    if (num_tiles == max_tiles) die("too many");
    if (strncmp(i, "Tile ", 5) != 0) die("syntax");
    struct tile* t = &tiles[num_tiles++];
    i = read_int(i + 5, &t->id);
    if (t->id == 0) die("zero id");
    if (strncmp(i, ":\n", 2) != 0) die("syntax");
    i += 2;
    // Assemble the tile.
    for (int y = 0; y < tile_size; y++) {
      unsigned short row = 0;
      for (int x = 0; x < tile_size; x++) {
        if (*i != '#' && *i != '.') die("grid");
        row = row << 1 | (*i++ == '#');
      }
      if (*i++ != '\n') die("line");
      t->cells[y] = row;
    }
    if (*i++ != '\n') die("gap");
    // Compute all of the tile edges.
    t->edges[0].top = t->cells[0];
    t->edges[0].right = column(t, 9);
    t->edges[0].bottom = t->cells[9];
    t->edges[0].left = column(t, 0);
    for (unsigned char transform = 0; transform < 8; transform++) {
      struct edges e = t->edges[0];
      if (transform & rotate_right) {
        const unsigned short temp = e.top;
        e.top = e.left;
        e.left = e.bottom;
        e.bottom = e.right;
        e.right = temp;
      }
      if (transform & flip_horizontal) {
        e.top = reverse(e.top) >> (16 - tile_size);
        e.bottom = reverse(e.bottom) >> (16 - tile_size);
        const unsigned short temp = e.left;
        e.left = e.right;
        e.right = temp;
      }
      if (transform & flip_vertical) {
        e.left = reverse(e.left) >> (16 - tile_size);
        e.right = reverse(e.right) >> (16 - tile_size);
        const unsigned short temp = e.top;
        e.top = e.bottom;
        e.bottom = temp;
      }
      t->edges[transform] = e;
    }
  }
}

// To find neighbouring edges, we'll interpret the edge values as integers and
// use them to index into the bucket array. We'll do this for each possible
// transform of each tile and then look for corner pieces among the buckets.
enum { max_ids = 2 };
struct bucket {
  unsigned short num_ids;
  unsigned short ids[max_ids];
};
struct bucket buckets[1 << tile_size];

static void add(unsigned short id, unsigned short value) {
  struct bucket* b = &buckets[value];
  if (b->num_ids == max_ids) die("too many matches");
  b->ids[b->num_ids++] = id;
}

static unsigned long long part1() {
  // Build a lookup table for finding tiles based on their edges.
  for (int i = 0; i < num_tiles; i++) {
    struct tile* t = &tiles[i];
    add(t->id, t->edges[0].top);
    add(t->id, t->edges[0].right);
    add(t->id, t->edges[0].bottom);
    add(t->id, t->edges[0].left);
    add(t->id, t->edges[flip_horizontal | flip_vertical].top);
    add(t->id, t->edges[flip_horizontal | flip_vertical].right);
    add(t->id, t->edges[flip_horizontal | flip_vertical].bottom);
    add(t->id, t->edges[flip_horizontal | flip_vertical].left);
  }
  // Find corners by looking for tiles that only have 2 matched edges.
  int num_corners = 0;
  unsigned short corners[4];
  for (int i = 0; i < num_tiles; i++) {
    struct tile* t = &tiles[i];
    int matches = 0;
    if (buckets[t->edges[0].top].num_ids == 2) matches++;
    if (buckets[t->edges[0].right].num_ids == 2) matches++;
    if (buckets[t->edges[0].bottom].num_ids == 2) matches++;
    if (buckets[t->edges[0].left].num_ids == 2) matches++;
    if (matches == 2) {
      if (num_corners == 4) die("too many corners");
      corners[num_corners++] = t->id;
    }
  }
  unsigned long long total = 1;
  for (int i = 0; i < 4; i++) total *= corners[i];
  return total;
}

int main() {
  read_input();
  print_int64(part1());
}
