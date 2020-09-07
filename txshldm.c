#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>

#include "txshldm.h"


const rank_value rank_values[RANKS_NUM] = {
					{'2', 2},
					{'3', 3},
					{'4', 4},
					{'5', 5},
					{'6', 6},
					{'7', 7},
					{'8', 8},
					{'9', 9},
					{'T', 10},
					{'J', 11},
					{'Q', 12},
					{'K', 13},
					{'A', 14}};

const suite_value suite_values[SUITES_NUM] = {
		{'h', 1},
		{'d', 2},
		{'c', 4},
		{'s', 8}};

const char *hand_value_name[] = {
		"HIGH_CARD",
		"PAIR",
		"TWO_PAIRS",
		"THREE_OF_A_KIND",
		"STRAIGHT",
		"FLUSH",
		"FULL_HOUSE",
		"FOUR_OF_A_KIND",
		"STRAIGHT_FLUSH",
		"ROYAL_FLUSH"
};

int txs_read_cards(char *line, txs_game *game) {
	int i;
	char *p;

	game->hands_num = 0;

	for (i = 0, p = line; i < COMMUNITY_CARDS_NUM; i++) {
		game->ccards[i].c[0] = *(p++);
		game->ccards[i].c[1] = *(p++);
	}

	for (i = 0; *p != 0 && *p != '\n'; i++) {
		p++;
		txs_hand empty_hand = {0};
		game->hands_num++;
		game->hands[i] = empty_hand;
		game->hands[i].cards[0].c[0] = *(p++);
		game->hands[i].cards[0].c[1] = *(p++);
		game->hands[i].cards[1].c[0] = *(p++);
		game->hands[i].cards[1].c[1] = *(p++);
	}

	return SUCCESS;
}

hand_value evaluate_five(txs_card *five, int kickers[3]) {
	//printf("%s()\n", __func__);
	int rank5[5] = {0};
	int suite5[5] = {0};
	int i;

	for (i = 0; i < 5; i++) {
		int rank_val = get_rank_value(five[i].c[0]);
		int suite_val = get_suite_value(five[i].c[1]);
		rank5[i] = 1 << rank_val;
		suite5[i] = 1 << suite_val;
		//printf("[%c %d]", five[i].c[0], rank5[i]);
		//printf("[%c %d] ", five[i].c[1], suite5[i]);
	}
	//printf("\n");
	if (is_royal_flush(rank5, suite5, kickers))
		return ROYAL_FLUSH;
	else if (is_straight_flush(rank5, suite5, kickers))
		return STRAIGHT_FLUSH;
	else if (is_four_of_a_kind(rank5, suite5, kickers))
		return FOUR_OF_A_KIND;
	else if (is_full_house(rank5, suite5, kickers))
		return FULL_HOUSE;
	else if (is_flush(rank5, suite5, kickers))
		return FLUSH;
	else if (is_straight(rank5, suite5, kickers))
		return STRAIGHT;
	else if (is_three_of_a_kind(rank5, suite5, kickers))
		return THREE_OF_A_KIND;
	else if (is_two_pairs(rank5, suite5, kickers))
		return(TWO_PAIRS);
	else if (is_pair(rank5, suite5, kickers))
		return PAIR;
	else {
		is_high_card(rank5, suite5, kickers);
		return HIGH_CARD;
	}

	return 0;
}

int is_royal_flush(int *r5, int *s5, int kickers[3]) {
	int i;
	int ranks = 0;
	if ( !(s5[0] & s5[1] & s5[2] & s5[3] & s5[4]) )
		return 0;
	for (i = 0; i < 5; i++){
		ranks |= r5[i];
	}
	for (i = 0; i < 5; i++){
		if (!(ranks & 1 << (14 - i)))
			return 0;
	}
	kickers[0] = ranks;
	return 1;
}

int is_straight_flush(int *r5, int *s5, int kickers[3]){
	int i;
	int b;
	int n;
	int ranks = 0;

	if ( !(s5[0] & s5[1] & s5[2] & s5[3] & s5[4]) )
		return 0;
	for (i = 0; i < 5; i++){
		ranks |= r5[i];
		//for straight with Ace = 1
		if (r5[i] == 1 << 14)
			ranks |= 1 << 1;
	}

	for (b = 14, n = 0; b > 0 ; b--) {

		if (ranks & 1 << b)
			n++;
		else
			n = 0;

		if (n == 5)
			break;
	}

	//remove A from value 14 in straight flush A23456
	if((n == 5) && (ranks & 1 << 2))
		ranks &= ~(1 << 14);
	else
		ranks &= ~(1 << 1);

	kickers[0] = ranks;

	return (n == 5);
}
int is_four_of_a_kind(int *r5, int *s5, int kickers[3]) {
	int i, j, n4;
	for (i = 0; i < 5; i++) {
		n4 = ~0;
		for (j = 0; j < 5; j++) {
			if (i == j)
				continue;
			n4 &= r5[j];
		}
		if (n4) {
			kickers[0] = n4;
			kickers[2] = (r5[0] | r5[1] | r5[2] | r5[3] | r5[4]) & (~n4);
			return 1;
		}
	}
	return 0;
}

int is_full_house(int *r5, int *s5, int kickers[3]) {
	int i,j;
	int three = 0;
	int two = 0;
	int n;

	for (i = 14; i > 0; i--) {
		n = 0;
		for (j = 0; j < 5; j++) {
			if (r5[j] & 1 << i)
				n++;
		}
		if (n == 3)
			three = i;
		else if (n == 2)
			two = i;
	}
	if (three && two) {
		kickers[0] = 1 << three;
		kickers[1] = 1 << two;
		kickers[2] = (r5[0] | r5[1] | r5[2] | r5[3] | r5[4]) &
				~(1 << three) & ~(1 << two);
		return 1;
	}

	return 0;
}

int is_flush(int *r5, int *s5, int kickers[3]) {
	int i, f;
	int ranks = 0;
	for (i = 0, f = ~0; i < 5; i++)
		f &= s5[i];

	if (!f)
		return 0;

	for (i = 0; i < 5; i++){
		ranks |= r5[i];
	}

	kickers[0] = ranks;

	return 1;
}

int is_straight(int *r5, int *s5, int kickers[3]) {
	int i;
	int b;
	int n;
	int ranks = 0;

	for (i = 0; i < 5; i++){
		ranks |= r5[i];

		//for straight with Ace = 1
		if (r5[i] == 1 << 14)
			ranks |= 1 << 1;
	}

	for (b = 14, n = 0; b > 0 ; b--) {

		if (ranks & 1 << b)
			n++;
		else
			n = 0;

		if (n == 5)
			break;
	}

	//remove A from value 14 in straight flush A23456
	if((n == 5) && (ranks & 1 << 2))
		ranks &= ~(1 << 14);
	else
		ranks &= ~(1 << 1);

	kickers[0] = ranks;

	return (n == 5);
}

int is_three_of_a_kind(int *r5, int *s5, int kickers[3]) {
	int i,j;
	int three = 0;
	int n;

	for (i = 14; i > 0; i--) {
		n = 0;
		for (j = 0; j < 5; j++) {
			if (r5[j] & 1 << i)
				n++;
		}
		if (n == 3)
			three = i;
	}
	if (three) {
		kickers[0] = 1 << three;
		kickers[2] = (r5[0] | r5[1] | r5[2] | r5[3] |r5[4]) & ~(1 << three);
		return 1;
	}

	return 0;
}
int is_two_pairs(int *r5, int *s5, int kickers[3]) {
	int i,j;
	int pair1 = 0;
	int pair2 = 0;
	int n;

	for (i = 14; i > 0; i--) {
		n = 0;
		for (j = 0; j < 5; j++) {
			if (r5[j] & 1 << i)
				n++;
		}
		if ( n == 2) {
			if (!pair1)
				pair1 = i;
			else
				pair2 = i;
		}
	}

	if (pair1 && pair2) {
		if (pair1 > pair2) {
			kickers[0] = 1 << pair1;
			kickers[1] = 1 << pair2;
		}
		else {
			kickers[0] = 1 << pair2;
			kickers[1] = 1 << pair1;
		}
		kickers[2] = (r5[0] | r5[1] | r5[2] | r5[3] | r5[4]) &
				~(1 << pair1) & ~(1 << pair2);
		return 1;
	}

	return 0;
}

int is_pair(int *r5, int *s5, int kickers[3]) {
	int i,j;
	int pair = 0;
	int n;

	for (i = 14; i > 0; i--) {
		n = 0;
		for (j = 0; j < 5; j++) {
			if (r5[j] & 1 << i)
				n++;
		}
		if ( n == 2) {
				pair = i;
		}
	}

	if (!pair)
		return 0;

	kickers[0] = 1 << pair;
	kickers[2] = (r5[0] | r5[1] | r5[2] | r5[3] |r5[4]) & ~(1 << pair);

	return 1;
}

int is_high_card(int *r5, int *s5, int kickers[3]) {
	int i;

	for(i = 0; i < 5; i++) {
		kickers[0] |= r5[i];
	}

	return 1;
}

int evaluate_hands(txs_game *game) {
	//printf("%s()\n", __func__);
	int i;
	for (i = 0; i < game->hands_num; i++)
		evaluate_hand(game, i);

	return 0;
}

int evaluate_hand(txs_game *game, int hand_num) {
	//printf("%s()\n", __func__);
	if (game->omaha)
		evaluate_hand_Omaha(game, hand_num);
	else
		evaluate_hand_Texas(game, hand_num);
	return 0;
}

int evaluate_hand_Texas(txs_game *game, int hand_num) {
	//printf("%s()\n", __func__);
	int i, j;
	int kickers[3];
	txs_card cards7[7] = {0};
	txs_card cards5[5] = {0};
	//txs_hand tmp_hand = {0};
	hand_value val;
	//memcpy(cards7, game->ccards, sizeof(txs_card) * 5);
	for (i = 0 ; i < 5; i++) {
		cards7[i] = game->ccards[i];
	}

	cards7[5] = game->hands[hand_num].cards[0];
	cards7[6] = game->hands[hand_num].cards[1];

	//printf("card7:");
	//for (i = 0; i < 7; i++)
	//	printf("[%c%c]", cards7[i].c[0], cards7[i].c[1]);
	//printf("\n");

	for (i = 0; i < 7; i++) {
		for (j = 0; j < 7; j++) {
			if (i == j)
				continue;
			int i5, i7;

			//printf("card7:");
			//int c7;
			//for (c7 = 0; c7 < 7; c7++)
			//	printf("[%c%c]", cards7[c7].c[0], cards7[c7].c[1]);
			//printf("\n");

			//printf("i:%d j:%d card5:", i, j);
			//printf("card5:");
			for (i5 = 0, i7 = 0; i7 < 7; i7++) {
				if (i7 == i || i7 == j){
					continue;
				}
				cards5[i5] = cards7[i7];
				//printf("i5:%d,i7:%d", i5, i7);
				//printf("[%d:%d][%c%c]", i5, i7, cards5[i5].c[0], cards5[i5].c[1]);
				//printf("[%c%c]", cards5[i5].c[0], cards5[i5].c[1]);
				i5++;
			}
			//printf("\n");

			//printf("i:%d, j:%d", i, j);
			val = evaluate_five(cards5, kickers);
			if ( val > game->hands[hand_num].value) {
				memcpy(game->hands[hand_num].best_combination, cards5, sizeof(txs_card) * 5);
				memcpy(game->hands[hand_num].kickers, kickers, sizeof(int) * 3);
				game->hands[hand_num].value = val;
			} else if	(val == game->hands[hand_num].value) {
				int k;
				for (k = 0; k < 3; k++) {
					if (kickers[k] > game->hands[hand_num].kickers[k]) {
						memcpy(game->hands[hand_num].best_combination, cards5, sizeof(txs_card) * 5);
						memcpy(game->hands[hand_num].kickers, kickers, sizeof(int) * 3);
						game->hands[hand_num].value = val;
						break;
					}
				}
			}
		}
	}

	return 0;
}


int evaluate_hand_Omaha(txs_game *game, int hand_num) {

	return 0;
}

int compare_hands(const void *h1, const void *h2) {
	//printf("%s()\n", __func__);
	int i;
	const txs_hand *hand1 = (const txs_hand *) h1;
	const txs_hand *hand2 = (const txs_hand *) h2;

	if (hand1->value != hand2->value) {
		return hand1->value - hand2->value;
	} else {
		for (int i = 0; i < 3; i++) {
			if (hand1->kickers[i] != hand2->kickers[i])
				return hand1->kickers[i] - hand2->kickers[i];
		}
	}

	//compare ahphabetically
	for (i = 0; i < 2; i++) {//Todo: 4 for Omaha
		if (hand1->cards[i].c[0] != hand2->cards[i].c[0])
			return (hand1->cards[i].c[0] - hand2->cards[i].c[0]);
		else if (hand1->cards[i].c[1] != hand2->cards[i].c[1])
			return (hand1->cards[i].c[1] - hand2->cards[i].c[1]);
	}

	return 0;
}

int sort_hands(txs_game *game) {
	//printf("%s()\n", __func__);
	memcpy(game->hands_sorted, game->hands, sizeof(txs_hand) * game->hands_num);
	//int size = sizeof(arr) / sizeof(arr[0]);
	qsort(game->hands_sorted, game->hands_num, sizeof(txs_hand), compare_hands);
	return 0;
}



void print_ccards(txs_game *game) {
	int i;
	printf("ccards: ");
	for (i = 0; i < COMMUNITY_CARDS_NUM; i++) {
		printf("%c%c", game->ccards[i].c[0], game->ccards[i].c[1]);
	}
	printf("\n");
}

void print_hands(txs_game *game) {
	int i, j;
	printf("hands: ");
	for (i = 0; i < game->hands_num; i++) {
		for (j = 0; j < (game->omaha ? 4 : 2); j++) {
			txs_card card = game->hands[i].cards[j];
			printf("%c%c", card.c[0], card.c[1]);
		}

		txs_hand hand = game->hands[i];
		printf("k:%d ", hand.kickers[0]);

		if (i < game->hands_num - 1) {
			printf(" ");
		}
	}
	printf("\n");
}

void print_sorted_hands(txs_game *game) {
	int i, j;
	//printf("sorted hands: ");
	for (i = 0; i < game->hands_num; i++) {
		for (j = 0; j < (game->omaha ? 4 : 2); j++) {
			txs_card card = game->hands_sorted[i].cards[j];
			printf("%c%c", card.c[0], card.c[1]);
		}

		if (i < game->hands_num - 1) {
			int equal = 1;
			if (game->hands_sorted[i].value !=  game->hands_sorted[i+1].value) {
				equal = 0;
			} else {
				int k;
				for(k = 0; k < 3; k++) {
					if (game->hands_sorted[i].kickers[k] !=  game->hands_sorted[i+1].kickers[k]) {
						equal = 0;
						break;
					}
				}
			}
			if (equal)
				printf("=");
			else
				printf(" ");
		}
	}
	printf("\n");
}

void print_best_combinations(txs_game *game) {
	int i;
	printf("best: \n");
	for (i = 0; i < game->hands_num; i++) {
		char tmp[11] = {0};
		snprintf(tmp, 11, "%s", (char*)game->hands_sorted[i].best_combination);
		printf("%s - %s\n", tmp, hand_value_name[game->hands_sorted[i].value]);
	}
	printf("\n");
}

int get_rank_value(char rank) {
	//printf("%s()\n", __func__);
	int i;
	for (i = 0; i < RANKS_NUM; i++) {
		if (rank_values[i].rank == rank)
			return rank_values[i].value;
	}
	//printf("incorrect rank: [%d]\n", rank);
	//exit(1);
	return 0;
}

int get_suite_value(char suite) {
	//printf("%s()\n", __func__);
	int i;
	for (i = 0; i < SUITES_NUM; i++) {
		if (suite_values[i].suite == suite)
			return suite_values[i].value;
	}
	printf("incorrect suite character: [%c]", suite);
	exit(1);
}

const char *get_hand_value_name(hand_value val) {
	return hand_value_name[val];
}
