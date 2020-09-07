#include <argp.h>
#include <stdio.h>
#include <stdlib.h>

#include "txshldm.h"

static char doc[] =
		"poker - programm for sorting Texas Holdem and Omaha Holdem Hands"
"\vTo pass input from file:\n"
"		cat ./test.txt | ./poker\n"
"		cat ./testOmaha.txt | ./poker --omaha";

static char args_doc[] = "";

#define OPT_OMAHA  1

static struct argp_option options[] = {
		{"omaha",  OPT_OMAHA, 0,       OPTION_ARG_OPTIONAL, "Omaha option" },
		{ 0 }
};

struct arguments
{
	int omaha;
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
	default:
		return ARGP_ERR_UNKNOWN;
	}
	return 0;
}

static struct argp argp = { options, parse_opt, args_doc, doc };

int main(int argc, char **argv) {
	struct arguments arguments;
	char line[200];
	txs_game game = {0};

	argp_parse (&argp, argc, argv, 0, 0, &arguments);

	if (arguments.omaha == 1) {
		game.omaha = 1;
	}

	while (fgets(line, sizeof(line), stdin) != NULL) {
		txs_read_cards(line, &game);
		evaluate_hands(&game);
		//print_ccards(&game);
		//print_hands(&game);
		sort_hands(&game);
		print_sorted_hands(&game);
		//print_best_combinations(&game);S
	}

	return 0;
}
