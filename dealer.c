#include <argp.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

static char doc[] =
		"dealer - programm for dealing hands for poker program"
		"\vTo pass input from file:\n"
		"		./dealer | ./poker --lines=10\n"
		"		./dealer | ./poker --lines=2500 --omaha\n"
		"		./dealer | ./poker --lines=1 --hands=2\n"
		"where 'lines' is number lines to generate";

static char args_doc[] = "";

static char deck[52 * 2] = "AhKhQhJhTh9h8h7h6h5h4h3h2h"
		"AdKdQdJdTd9d8d7d6d5d4d3d2d"
		"AdKdQdJdTd9d8d7d6d5d4d3d2d"
		"AsKsQsJsTs9s8s7s6s5s4s3s2s";


#define OPT_OMAHA	1
#define OPT_LINES	2
#define OPT_HANDS	3

static struct argp_option options[] = {
		{"omaha",  OPT_OMAHA, 0,       OPTION_ARG_OPTIONAL, "Omaha option" },
		{"lines",  OPT_LINES , "COUNT",       0, "Number of lines to generate" },
		{"hands",  OPT_HANDS , "COUNT",       0, "Number of hands in line" },
		{ 0 }
};

struct arguments
{
	int omaha;
	int lines;
	int hands;
};

static error_t
parse_opt (int key, char *arg, struct argp_state *state)
{
	struct arguments *arguments = state->input;

	switch (key)
	{
	case OPT_OMAHA:
		arguments->omaha = 1;
		break;
	case OPT_LINES:
		arguments->lines = (arg ? atoi (arg) : 1);
		break;
	case OPT_HANDS:
		arguments->hands = (arg ? atoi (arg) : 0);
		break;
	case ARGP_KEY_NO_ARGS:
		break;
	default:
		return ARGP_ERR_UNKNOWN;
	}
	return 0;
}

static struct argp argp = { options, parse_opt, args_doc, doc };

//return number of card
int deal_card(char *cards_dealed);

int main(int argc, char **argv) {
	struct arguments arguments;
	int p, l;
	char line[200] = {' '};
	line[sizeof(line) - 1] = 0;
	char cards_dealed[52 * 2 + 1] = {' '};
	cards_dealed[sizeof(cards_dealed) - 1] = 0;

	arguments.lines = 1;
	argp_parse (&argp, argc, argv, 0, 0, &arguments);

	srand(time(0));

	for (l = 0; l < arguments.lines; l++) {
		//printf("line[%d]\n", l + 1);
		int c, h, hands, players;

		p = 0;

		memset(cards_dealed, ' ' , sizeof(cards_dealed));
		memset(line, ' ', (sizeof(line) - 1));

		//generate community cards
		for (c = 0; c < 5; c++) {

			int n;
			n = deal_card(cards_dealed);;
			line[p++] = deck[n * 2];
			line[(p++)] = deck[n * 2 + 1];
		}

		line[p++] = ' ';

		if (!arguments.hands) {
			players = (arguments.omaha) ? 11 : 23;
			hands = (int) (rand() % (players - 2) + 2 + 1);
		} else if (arguments.omaha) {
			hands = (arguments.hands <= 11 && arguments.hands >= 2) ? arguments.hands : 2;
		} else {
			hands = (arguments.hands <= 23 && arguments.hands >= 2) ? arguments.hands : 2;
		}

		for (h = 0; h < hands; h++) {
			int n;
			int c, cards;
			cards = (arguments.omaha) ? 4 : 2;
			for (c = 0; c < cards; c++) {
				n = deal_card(cards_dealed);
				line[p++] = deck[n * 2];
				line[p++] = deck[n * 2 + 1];
			}
			if (h < hands - 1)
				line[p++] = ' ';
		}

		line[p++] = '\n';
		line[p++] = '\0';
		printf("%s", line);;
	}

	return 0;
}

int deal_card(char *cards_dealed) {
	int i;
	int num;
	int cards;

	for (i = 0, cards = 52; i < 52; i++) {
		if (cards_dealed[i * 2] != ' ' && cards_dealed[i * 2] != 0)
			cards--;
		if (cards < 1) {
			printf("Error: This should not happen. No cards left to deal\n");
			exit(1);
		}
	}

	while(1) {
		num = (int) (rand() % 52);
		if (cards_dealed[num * 2] == 0 || cards_dealed[num * 2] == ' ') {
			cards_dealed[num * 2] = deck[num * 2];
			cards_dealed[num * 2 + 1] = deck[num * 2 + 1];
			return num;
		}
	}

	printf("Error: this should not happen\n");
	exit(1);
	return 0;
}

