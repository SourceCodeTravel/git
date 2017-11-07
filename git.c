#include "debug.h"

int handle_options(char ***argv, int *argc)
{
	while (*argc > 0) {
		break;
	}
	return 0;
}

int cmd_main(int argc, char **argv)
{
	argv++;
	argc--;
	handle_options(&argv, &argc);
	if (argc > 0) {
		DEBUG(0);
	} else {
		printf("usage:\n");
	}
	return 0;
}

