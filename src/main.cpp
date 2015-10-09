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
		if (c == '\n') printf("\n> ");
		state.evalChar(c);
	}

	return EXIT_SUCCESS;
}
