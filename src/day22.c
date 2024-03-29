// Input: The cards in each of two player hands.
// Part 1: Find the score of the winning player in a game of Combat.
// A game of Combat proceeds as follows:
//   Each player turns over their top card. The player with the higher value
//   wins the round. The winner receives both cards and places them on the
//   bottom of their own pile such that the winner's card is above the loser's
//   card. Play proceeds until one player holds every card, at which point they
//   win. Their score is computed by multiplying each card by its position in
//   the hand (starting at 1 for the bottom card) and taking the sum.
// Part 2: Find the score of the winning player in a game of Recursive Combat.
// A game of Recursive Combat proceeds as follows:
//   If the two players' hands are exactly the same as they have been at an
//   earlier point in the same game, player 1 wins. Otherwise, each player draws
//   and plays their top card. If either player has fewer cards than the value
//   of the card they just played, then the winner of the round is the one with
//   the higher valued card. Otherwise, each player copies a subset of their
//   deck of cards equal in size to the value of the card they played, and a new
//   game of Recursive Combat is played to determine the winner of the round.
//   The winner of that game wins the round. Upon winning a round, the winner
//   takes both cards (as they do in Combat). The game continues until one
//   player holds all the cards.
//
// Approach: part 1 is straightforward: we simulate the game one turn at a time
// until it naturally concludes. For part 2, we have the additional constraint
// that we need to identify hands that we have seen before. To do this in
// a memory-efficient manner, we will simulate two copies of the game alongside
// each other and use the "tortoise and the hare" trick to detect any cycles. If
// the hare terminates, we don't have a cycle and proceed as described above.
// Otherwise, the tortoise hands and the hare hands will eventually match and we
// will exit with a win for player 1. This technique has a single drawback: it
// will not necessarily detect the cycle right at the start. This doesn't matter
// unless the outermost game terminates due to a cycle, so we will assume that
// this is impossible.

#include "util/die.h"
#include "util/memcpy.h"
#include "util/memmove.h"
#include "util/print_int.h"
#include "util/read_int8.h"
#include "util/strncmp.h"

enum { max_cards = 64 };
struct hand {
  unsigned char cards[max_cards];
  int num_cards;
};
static struct hand input_hands[2];

static void read_input() {
  char buffer[1024];
  const int length = read(STDIN_FILENO, buffer, sizeof(buffer));
  if (length <= 0) die("read");
  if (buffer[length - 1] != '\n') die("newline");
  const char* i = buffer;
  const char* const end = buffer + length;
  // Keep the count of cards that we have seen across both players. If this
  // exceeds max_cards, we won't have space in a single player's hand for them
  // to win the game.
  int num_cards = 0;
  for (int player = 1; player <= 2; player++) {
    if (strncmp(i, "Player ", 7) != 0) die("syntax");
    i += 7;
    if (*i - '0' != player) die("wrong player");
    i++;
    if (strncmp(i, ":\n", 2) != 0) die("syntax");
    i += 2;
    struct hand* h = &input_hands[player - 1];
    while (i != end && *i != '\n') {
      if (num_cards == max_cards) die("too many");
      i = read_int(i, &h->cards[h->num_cards++]);
      if (*i != '\n') die("line");
      i++;
    }
    i++;
  }
}

struct circular_buffer {
  unsigned char cards[max_cards];
  unsigned begin, size;
};

static unsigned char pop_front(struct circular_buffer* b) {
  const unsigned char value = b->cards[b->begin];
  b->begin = (b->begin + 1) % max_cards;
  b->size--;
  return value;
}

static void push_back(struct circular_buffer* b, unsigned char value) {
  const int i = (b->begin + b->size) % max_cards;
  b->cards[i] = value;
  b->size++;
}

static unsigned char pop_back(struct circular_buffer* b) {
  b->size--;
  const int i = (b->begin + b->size) % max_cards;
  return b->cards[i];
}

static void init_hands(struct circular_buffer* hands) {
  memcpy(hands[0].cards, input_hands[0].cards, input_hands[0].num_cards);
  hands[0].begin = 0;
  hands[0].size = input_hands[0].num_cards;
  memcpy(hands[1].cards, input_hands[1].cards, input_hands[1].num_cards);
  hands[1].begin = 0;
  hands[1].size = input_hands[1].num_cards;
}

static int score(struct circular_buffer* winner) {
  int points = 0;
  for (int i = 1; winner->size; i++) points += i * pop_back(winner);
  return points;
}

static int part1() {
  struct circular_buffer hands[2];
  init_hands(hands);
  // Play the game.
  while (hands[0].size && hands[1].size) {
    const unsigned char a = pop_front(&hands[0]);
    const unsigned char b = pop_front(&hands[1]);
    if (a < b) {
      push_back(&hands[1], b);
      push_back(&hands[1], a);
    } else if (a > b) {
      push_back(&hands[0], a);
      push_back(&hands[0], b);
    } else {
      die("draw");
    }
  }
  // Count up the score.
  return score(hands[0].size ? &hands[0] : &hands[1]);
}

static struct circular_buffer* recursive_combat(struct circular_buffer* hands);

// Runs a single round of recursive combat. Returns true if the game finishes as
// a result of this round.
static bool recursive_combat_round(struct circular_buffer* hands) {
  const unsigned char a = pop_front(&hands[0]);
  const unsigned char b = pop_front(&hands[1]);
  bool a_wins;
  if (a <= hands[0].size && b <= hands[1].size) {
    // Recursive game.
    struct circular_buffer new_hands[2] = {hands[0], hands[1]};
    new_hands[0].size = a;
    new_hands[1].size = b;
    a_wins = recursive_combat(new_hands) == &new_hands[0];
  } else {
    // Direct combat.
    a_wins = a > b;
  }
  if (a_wins) {
    push_back(&hands[0], a);
    push_back(&hands[0], b);
    return hands[1].size == 0;
  } else {
    push_back(&hands[1], b);
    push_back(&hands[1], a);
    return hands[0].size == 0;
  }
}

// Returns true if two circular buffers have the same contents.
static bool buffers_equal(
    struct circular_buffer* l, struct circular_buffer* r) {
  if (l->size != r->size) return false;
  int i = l->begin, j = r->begin;
  int end = i + l->size;
  if (end >= max_cards) end -= max_cards;
  while (i != end) {
    if (l->cards[i] != r->cards[j]) return false;
    i++;
    j++;
    if (i == max_cards) i = 0;
    if (j == max_cards) j = 0;
  }
  return true;
}

static struct circular_buffer* recursive_combat(
    struct circular_buffer* hare_hands) {
  struct circular_buffer hands[] = {hare_hands[0], hare_hands[1]};
  while (true) {
    if (recursive_combat_round(hare_hands) ||
        recursive_combat_round(hare_hands)) {
      // The hare has finished the game.
      break;
    }
    recursive_combat_round(hands);
    if (buffers_equal(&hands[0], &hare_hands[0]) &&
        buffers_equal(&hands[1], &hare_hands[1])) {
      // The tortoise and the hare met up again: there is a cycle in the hands.
      return &hare_hands[0];
    }
  }
  return hare_hands[0].size ? &hare_hands[0] : &hare_hands[1];
}

static int part2() {
  struct circular_buffer hands[2];
  init_hands(hands);
  return score(recursive_combat(hands));
}

int main() {
  read_input();
  print_int(part1());
  print_int(part2());
}
