#include "util/die.h"
#include "util/memcpy.h"
#include "util/memset.h"
#include "util/print_int64.h"
#include "util/read_int16.h"
#include "util/strncmp.h"

// Composable transformations:
// +-one-+ +-rouf+ +-eno-+ +four-+ +eerht+ +-two-+ +three+ +-owt-+
// |     | |     e |     | e     | f     | |     e |     f t     |
// r  0  t o  1  e t  0  r e  1  o o  0  o o  1  e o  0  o h  1  e
// u  0  w n  0  r w  1  u r  1  n u  0  w n  0  r w  1  u r  1  n
// o  0  o e  0  h o  0  o h  0  e r  1  t e  1  h t  1  r e  1  o
// f     | |     t |     f t     | |     | |     t |     | e     |
// +eerht+ +-two-+ +three+ +-owt-+ +-one-+ +-rouf+ +-eno-+ +four-+
enum transformation {
  flip_diagonally = 1,  // Flip diagonally
  flip_horizontally = 2,  // Flip the tile horizontally
  flip_vertically = 4,  // Flip the tile vertically
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
static unsigned short column(const struct tile* t, int c) {
  unsigned short value = 0;
  for (int i = 0; i < tile_size; i++) {
    value = value << 1 | ((t->cells[i] >> (9 - c)) & 1);
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
    i = read_int16(i + 5, &t->id);
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
      if (transform & flip_diagonally) {
        const unsigned short a = e.top;
        e.top = e.left;
        e.left = a;
        const unsigned short b = e.bottom;
        e.bottom = e.right;
        e.right = b;
      }
      if (transform & flip_horizontally) {
        e.top = reverse(e.top) >> (16 - tile_size);
        e.bottom = reverse(e.bottom) >> (16 - tile_size);
        const unsigned short temp = e.left;
        e.left = e.right;
        e.right = temp;
      }
      if (transform & flip_vertically) {
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
struct tile* corners[4];

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
    add(t->id, t->edges[flip_horizontally | flip_vertically].top);
    add(t->id, t->edges[flip_horizontally | flip_vertically].right);
    add(t->id, t->edges[flip_horizontally | flip_vertically].bottom);
    add(t->id, t->edges[flip_horizontally | flip_vertically].left);
  }
  // Find corners by looking for tiles that only have 2 matched edges.
  int num_corners = 0;
  for (int i = 0; i < num_tiles; i++) {
    struct tile* t = &tiles[i];
    int matches = 0;
    if (buckets[t->edges[0].top].num_ids == 2) matches++;
    if (buckets[t->edges[0].right].num_ids == 2) matches++;
    if (buckets[t->edges[0].bottom].num_ids == 2) matches++;
    if (buckets[t->edges[0].left].num_ids == 2) matches++;
    // Any tile in a rectangular grid will have 2 neighbours (corner),
    // 3 neighbours (edge), or 4 neighbours (inner).
    if (matches < 2) die("not enough matches");
    if (matches == 2) {
      if (num_corners == 4) die("too many corners");
      corners[num_corners++] = t;
    }
  }
  if (num_corners < 4) die("not enough corners");
  unsigned long long total = 1;
  for (int i = 0; i < 4; i++) total *= corners[i]->id;
  return total;
}

struct oriented_tile {
  const struct tile* tile;
  unsigned char orientation;
};

static struct oriented_tile top_left_transform(const struct tile* tile) {
  for (int i = 0; i < 8; i++) {
    const struct edges* e = &tile->edges[i];
    if (buckets[e->top].num_ids == 1 && buckets[e->left].num_ids == 1) {
      return (struct oriented_tile){.tile = tile, .orientation = i};
    }
  }
  die("invalid corner");
}

struct tile* get_tile(int id) {
  for (int i = 0; i < num_tiles; i++) {
    if (tiles[i].id == id) return &tiles[i];
  }
  die("no such tile");
}

enum { grid_size = 16 };
static unsigned char grid[8 * grid_size][grid_size];

static void copy_tile(struct oriented_tile t, int tile_x, int tile_y) {
  // Assemble the suitably oriented tile.
  unsigned char temp[8];
  if (t.orientation & flip_diagonally) {
    for (int y = 0; y < 8; y++) temp[y] = (column(t.tile, y + 1) >> 1) & 0xFF;
  } else {
    for (int y = 0; y < 8; y++) temp[y] = (t.tile->cells[y + 1] >> 1) & 0xFF;
  }
  if (t.orientation & flip_horizontally) {
    for (int y = 0; y < 8; y++) temp[y] = reverse(temp[y]) >> 8;
  }
  if (t.orientation & flip_vertically) {
    for (int y = 0; y < 4; y++) {
      unsigned char x = temp[y];
      temp[y] = temp[7 - y];
      temp[7 - y] = x;
    }
  }
  // Put the tile into the grid.
  for (int y = 0; y < 8; y++) grid[8 * tile_y + y][tile_x] = temp[y];
}

static struct oriented_tile next_right(struct oriented_tile input) {
  const struct edges* e = &input.tile->edges[input.orientation];
  const struct bucket* b = &buckets[e->right];
  if (b->num_ids == 1) return (struct oriented_tile){.tile = NULL};
  const struct tile* next =
      get_tile(b->ids[0] == input.tile->id ? b->ids[1] : b->ids[0]);
  unsigned char orientation = 0;
  while (orientation < 8 && next->edges[orientation].left != e->right) {
    orientation++;
  }
  if (orientation == 8) die("no matching orientation");
  return (struct oriented_tile){.tile = next, .orientation = orientation};
}

static struct oriented_tile next_down(struct oriented_tile input) {
  const struct edges* e = &input.tile->edges[input.orientation];
  const struct bucket* b = &buckets[e->bottom];
  if (b->num_ids == 1) return (struct oriented_tile){.tile = NULL};
  const struct tile* next =
      get_tile(b->ids[0] == input.tile->id ? b->ids[1] : b->ids[0]);
  unsigned char orientation = 0;
  while (orientation < 8 && next->edges[orientation].top != e->bottom) {
    orientation++;
  }
  if (orientation == 8) die("no matching orientation");
  return (struct oriented_tile){.tile = next, .orientation = orientation};
}

enum { monster_width = 20, monster_height = 3 };

unsigned char transformed_grid[8 * grid_size][grid_size];

unsigned find_sea_monsters(int size) {
  //                   #     00000000 00000000 00100000    0x00 0x00 0x20
  // #    ##    ##    ### -> 10000110 00011000 01110000 -> 0x86 0x18 0x70
  //  #  #  #  #  #  #       01001001 00100100 10000000    0x49 0x24 0x80
  const unsigned char untranslated_sea_monster[3][4] = {
    {0x00, 0x00, 0x20, 0x00},
    {0x86, 0x18, 0x70, 0x00},
    {0x49, 0x24, 0x80, 0x00},
  };
  int monsters_found = 0;
  for (int offset_x = 0; offset_x < 8; offset_x++) {
    unsigned char sea_monster[3][4];
    for (int y = 0; y < 3; y++) {
      sea_monster[y][0] = untranslated_sea_monster[y][0] >> offset_x;
      for (int x = 1; x < 4; x++) {
        sea_monster[y][x] =
            untranslated_sea_monster[y][x - 1] << (8 - offset_x) |
            untranslated_sea_monster[y][x] >> offset_x;
      }
    }
    for (int y = 0, y_end = 8 * size - 3; y < y_end; y++) {
      for (int x = 0, x_end = size - 2; x < x_end; x++) {
        _Bool match = 1;
        for (int sy = 0; sy < 3; sy++) {
          for (int sx = 0; sx < 4; sx++) {
            const unsigned char s = sea_monster[sy][sx];
            match &= (transformed_grid[y + sy][x + sx] & s) == s;
          }
        }
        monsters_found += match;
      }
    }
  }
  return monsters_found;
}

static unsigned long long part2() {
  // Pick the first corner piece and orient it so that it can occupy the top
  // left corner of the grid.
  const struct oriented_tile top_left = top_left_transform(corners[0]);
  // Compute the grid dimensions. We will assume that the grid is always square.
  int size = 0;
  // Iterate to the right until we find the far corner of the top edge.
  for (struct oriented_tile t = top_left; t.tile; t = next_right(t)) size++;
  if (size * size != num_tiles) die("grid is not square");
  if (size > grid_size - 1) die("too big");  // We're relying on an empty border.
  // Place each tile in the grid.
  struct oriented_tile left = top_left;
  for (int y = 0; y < size; y++) {
    if (!left.tile) die("fell off bottom");
    struct oriented_tile tile = left;
    for (int x = 0; x < size; x++) {
      if (!tile.tile) die("fell off right");
      //debug_neighbours(tile);
      copy_tile(tile, x, y);
      tile = next_right(tile);
    }
    left = next_down(left);
  }
  // Search for the sea monster in the grid.
  int num_monsters = 0;
  for (int i = 0; i < 8; i++) {
    memset(transformed_grid, 0, sizeof(transformed_grid));
    if (i & flip_diagonally) {
      for (int y = 0; y < 8 * size; y++) {
        for (int x = 0; x < 8 * size; x++) {
          const _Bool value = (grid[x][y / 8] >> (7 - y % 8)) & 1;
          transformed_grid[y][x / 8] |= value << (7 - x % 8);
        }
      }
    } else {
      memcpy(transformed_grid, grid, sizeof(transformed_grid));
    }
    if (i & flip_horizontally) {
      for (int y = 0; y < 8 * size; y++) {
        for (int x1 = 0, x2 = size - 1; x1 <= x2; x1++, x2--) {
          const unsigned char temp = reverse(transformed_grid[y][x1]) >> 8;
          transformed_grid[y][x1] = reverse(transformed_grid[y][x2]) >> 8;
          transformed_grid[y][x2] = temp;
        }
      }
    }
    if (i & flip_vertically) {
      for (int y1 = 0, y2 = 8 * size - 1; y1 < y2; y1++, y2--) {
        unsigned char* row1 = transformed_grid[y1];
        unsigned char* row2 = transformed_grid[y2];
        for (int x = 0; x < size; x++) {
          const unsigned char temp = row1[x];
          row1[x] = row2[x];
          row2[x] = temp;
        }
      }
    }
    const unsigned found = find_sea_monsters(size);
    if (found) {
      num_monsters = found;
      break;
    }
  }
  // Count the populated cells.
  int count = 0;
  for (int y = 0; y < 8 * grid_size; y++) {
    for (int x = 0; x < grid_size; x++) {
      unsigned char c = grid[y][x];
      c = (c & 0x55) + ((c >> 1) & 0x55);
      c = (c & 0x33) + ((c >> 2) & 0x33);
      c = (c & 0x0F) + ((c >> 4) & 0x0F);
      count += c;
    }
  }
  // Subtract all cells which are part of a sea monster. We are assuming
  // that sea monsters do not overlap. If they do, we will underestimate the
  // true value.
  //                   #       1
  // #    ##    ##    ### -> + 8
  //  #  #  #  #  #  #       + 6 = 15
  // 2513 too high.
  return count - 15 * num_monsters;
}

int main() {
  read_input();
  print_int64(part1());
  print_int64(part2());
}
