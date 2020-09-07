#ifndef TXSHLDM_H
#define TXSHLDM_H

#define FAIL 1
#define SUCCESS 0

#define MAX_PLAYERS 22
#define COMMUNITY_CARDS_NUM 5
#define MAX_CARDS_ON_HAND 4
#define FIVE 5
#define RANKS_NUM 13
#define SUITES_NUM 4

typedef enum hand_value {
	HIGH_CARD,
	PAIR,
	TWO_PAIRS,
	THREE_OF_A_KIND,
	STRAIGHT,
	FLUSH,
	FULL_HOUSE,
	FOUR_OF_A_KIND,
	STRAIGHT_FLUSH,
	ROYAL_FLUSH
} hand_value;

typedef struct rank_value {
	char rank;
	char value;
}rank_value;

typedef struct suite_value {
	char suite;
	char value;
}suite_value;

typedef struct txs_card {
	char c[2];
} txs_card;

typedef struct txs_hand {
	txs_card cards[MAX_CARDS_ON_HAND];
	txs_card best_combination[FIVE];
	enum hand_value value;
	int kickers[3]; //e.g. [0] - rank of three, four, flash; [1] -rank of two, [2] - kickers
} txs_hand;

typedef struct txs_game {
	int omaha;
	int hands_num;
	txs_card ccards[COMMUNITY_CARDS_NUM];
	txs_hand hands[MAX_PLAYERS];
	txs_hand hands_sorted[MAX_PLAYERS];
} txs_game;

int txs_read_cards(char *line, txs_game *game);

void print_ccards(txs_game *game);

void print_hands(txs_game *game);
void print_sorted_hands(txs_game *game);
void print_best_combinations(txs_game *game);

int get_rank_value(char rank);

int get_suite_value(char suite);

hand_value evaluate_five(txs_card *five, int *kickers);

int evaluate_hands(txs_game *game);

int evaluate_hand(txs_game *game, int hand_num);

int evaluate_hand_Texas(txs_game *game, int hand_num);

int evaluate_hand_Omaha(txs_game *game, int hand_num);

int compare_hands(const void *hand1, const void *hand2);

int sort_hands(txs_game *game);

int is_royal_flush(int *rank5, int *suite5, int kickers[3]);
int is_straight_flush(int *rank5, int *suite5, int kickers[3]);
int is_four_of_a_kind(int *rank5, int *suite5, int kickers[3]);
int is_full_house(int *rank5, int *suite5, int kickers[3]);
int is_flush(int *rank5, int *suite5, int kickers[3]);
int is_straight(int *rank5, int *suite5, int kickers[3]);
int is_three_of_a_kind(int *rank5, int *suite5, int kickers[3]);
int is_two_pairs(int *rank5, int *suite5, int kickers[3]);
int is_pair(int *rank5, int *suite5, int kickers[3]);
int is_high_card(int *rank5, int *suite5, int kickers[3]);

const char *get_hand_value_name(hand_value val);

#endif
