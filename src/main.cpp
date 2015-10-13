#include "main.h"

int main(int argc, char **argv) {
	bool quitting = false;

	printf("> ");
	while (!quitting) {
		char c = getc(stdin);
		if (c == EOF) {
			printf("\n");
			break;
		}

		state.addCommand(c);
		state.evalChar(c);
		if (c == '\n') printf("> ");
	}

	return EXIT_SUCCESS;
}
