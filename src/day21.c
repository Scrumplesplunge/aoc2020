#include "util/die.h"
#include "util/memcpy.h"
#include "util/memset.h"
#include "util/popcount.h"
#include "util/print_int.h"
#include "util/strcmp.h"
#include "util/strncmp.h"

// Intern a string into a collection.
enum { max_size = 16 };
static unsigned char intern(const char* value, char (*values)[max_size],
                            int* num_values, int max_values) {
  for (int i = 0; i < *num_values; i++) {
    if (strcmp(value, values[i]) == 0) return i;
  }
  if (*num_values == max_values) die("too many");
  const int length = strlen(value);
  if (length >= max_size) die("too long");
  const int i = (*num_values)++;
  memcpy(&values[i], value, length + 1);
  return i;
}

enum { max_allergens = 16, max_ingredients = 256 };
static char allergens[max_allergens][max_size];
static char ingredients[max_ingredients][max_size];
static int num_allergens, num_ingredients;

enum { max_foods = 128 };
struct food {
  int num_ingredients, num_allergens;
  unsigned char ingredients[max_ingredients];
  unsigned char allergens[max_allergens];
};
static struct food foods[max_foods];
static int num_foods;

static void read_input() {
  char buffer[65536];
  const int length = read(STDIN_FILENO, buffer, sizeof(buffer));
  if (length <= 0) die("read");
  if (buffer[length - 1] != '\n') die("newline");
  char* i = buffer;
  char* const end = buffer + length;
  while (i != end) {
    if (num_foods == max_foods) die("too many foods");
    struct food* f = &foods[num_foods++];
    while (*i != '(') {
      if (f->num_ingredients == max_ingredients) die("too many ingredients");
      const char* ingredient_name = i;
      while (*i != ' ') i++;
      *i = '\0';
      f->ingredients[f->num_ingredients++] = intern(
          ingredient_name, ingredients, &num_ingredients, max_ingredients);
      i++;
    }
    if (strncmp(i, "(contains ", 10) != 0) die("syntax");
    i += 10;
    while (1) {
      if (f->num_allergens == max_allergens) die("too many allergens");
      const char* allergen_name = i;
      while (*i != ',' && *i != ')') i++;
      const _Bool done = *i == ')';
      if (!done && strncmp(i, ", ", 2) != 0) die("syntax");
      *i++ = '\0';
      f->allergens[f->num_allergens++] = intern(
          allergen_name, allergens, &num_allergens, max_allergens);
      if (done) break;
      i++;
    }
    if (*i != '\n') die("syntax");
    i++;
  }
}

enum { set_size = (max_ingredients + 31) / 32 };
// candidates[i] is the set of ingredients that might contain allergen i.
static unsigned candidates[max_allergens][set_size];

static int part1() {
  memset(candidates, 0xFF, sizeof(candidates));
  for (int i = 0; i < num_foods; i++) {
    const struct food* f = &foods[i];
    unsigned ingredient_set[set_size];
    memset(ingredient_set, 0, sizeof(ingredient_set));
    for (int j = 0; j < f->num_ingredients; j++) {
      const unsigned char ingredient = f->ingredients[j];
      ingredient_set[ingredient / 32] |= 1 << (ingredient % 32);
    }
    for (int j = 0; j < f->num_allergens; j++) {
      for (int k = 0; k < set_size; k++) {
        candidates[f->allergens[j]][k] &= ingredient_set[k];
      }
    }
  }
  // Debug output.
  // for (int i = 0; i < num_allergens; i++) {
  //   write(STDOUT_FILENO, allergens[i], strlen(allergens[i]));
  //   for (int j = 0; j < num_ingredients; j++) {
  //     if (candidates[i][j / 32] & (1 << (j % 32))) {
  //       write(STDOUT_FILENO, " ", 1);
  //       write(STDOUT_FILENO, ingredients[j], strlen(ingredients[j]));
  //     }
  //   }
  //   write(STDOUT_FILENO, "\n", 1);
  // }
  unsigned ingredient_set[set_size];
  memset(ingredient_set, 0, sizeof(ingredient_set));
  for (int i = 0; i < num_allergens; i++) {
    for (int j = 0; j < set_size; j++) {
      ingredient_set[j] |= candidates[i][j];
    }
  }
  int count = 0;
  for (int i = 0; i < num_foods; i++) {
    const struct food* f = &foods[i];
    for (int j = 0; j < f->num_ingredients; j++) {
      const unsigned char ingredient = f->ingredients[j];
      const _Bool can_have_allergen =
          ingredient_set[ingredient / 32] & (1 << (ingredient % 32));
      count += !can_have_allergen;
    }
  }
  return count;
}

static int possible_ingredients(const unsigned* set) {
  int count = 0;
  for (int i = 0; i < set_size; i++) count += popcount(set[i]);
  return count;
}

static unsigned char get_ingredient(const unsigned* set) {
  if (possible_ingredients(set) != 1) die("ambiguous");
  int i = 0;
  while (!set[i]) i++;
  int j = 0;
  while ((set[i] & (1 << j)) == 0) j++;
  return 32 * i + j;
}

static void part2() {
  _Bool changed = 1;
  while (changed) {
    changed = 0;
    // Find an allergen which is now uniquely identified.
    for (int i = 0; i < num_allergens; i++) {
      const unsigned* const row = candidates[i];
      const int count = possible_ingredients(row);
      if (count == 0) die("impossible");
      if (count == 1) {
        // Remove the ingredient from all other rows.
        for (int k = 0; k < num_allergens; k++) {
          if (i == k) continue;
          for (int j = 0; j < set_size; j++) {
            changed |= candidates[k][j] & row[j];
            candidates[k][j] &= ~row[j];
          }
        }
      }
    }
  }
  // dangerous[a] is the ingredient which contains allergen a.
  unsigned char dangerous[max_allergens];
  for (int i = 0; i < num_allergens; i++) {
    dangerous[i] = get_ingredient(candidates[i]);
  }
  // Sort the list of allergen names.
  unsigned char sorted_allergens[max_allergens];
  for (int i = 0; i < num_allergens; i++) sorted_allergens[i] = i;
  for (int i = 0; i < num_allergens; i++) {
    int min = i;
    for (int j = i + 1; j < num_allergens; j++) {
      if (strcmp(allergens[sorted_allergens[j]],
                 allergens[sorted_allergens[min]]) < 0) {
        min = j;
      }
    }
    const unsigned char temp = sorted_allergens[i];
    sorted_allergens[i] = sorted_allergens[min];
    sorted_allergens[min] = temp;
  }
  // Print out the list.
  char buffer[(max_size + 1) * max_allergens];
  char* o = buffer;
  int i = 0;
  while (1) {
    const char* name = ingredients[dangerous[sorted_allergens[i++]]];
    const int length = strlen(name);
    memcpy(o, name, length);
    o += length;
    if (i == num_allergens) break;
    *o++ = ',';
  }
  *o++ = '\n';
  write(STDOUT_FILENO, buffer, o - buffer);
}

int main() {
  read_input();
  print_int(part1());
  part2();
}
