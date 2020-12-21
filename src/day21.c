// Exit the program with a given error message.
__attribute__((noreturn)) static void die(const char* message) {
  write(STDERR_FILENO, message, strlen(message));
  write(STDERR_FILENO, "\n", 1);
  exit(1);
}

// Print an integer in decimal, followed by a newline.
static void print_int(int x) {
  const _Bool negative = x < 0;
  if (!negative) x = -x;
  char buffer[16];
  buffer[15] = '\n';
  int i = 15;
  do {
    --i;
    buffer[i] = '0' - (x % 10);
    x /= 10;
  } while (x);
  if (negative) buffer[--i] = '-';
  write(STDOUT_FILENO, buffer + i, 16 - i);
}

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
char allergens[max_allergens][max_size], ingredients[max_ingredients][max_size];
int num_allergens, num_ingredients;

enum { max_foods = 128 };
struct food {
  int num_ingredients, num_allergens;
  unsigned char ingredients[max_ingredients];
  unsigned char allergens[max_allergens];
};
struct food foods[max_foods];
int num_foods;

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

static int part1() {
  // candidates[i] is the set of ingredients that might contain allergen i.
  enum { set_size = (max_ingredients + 31) / 32 };
  unsigned candidates[max_allergens][set_size];
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

int main() {
  read_input();
  print_int(part1());
}
