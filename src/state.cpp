#include "state.h"

bool parse(char *cmds) {
	assert(cmds != NULL);

	// while the command is not null
	for (char c = *(cmds++); c != 0; c = *(cmds++)) {
		switch (c) {
			default:
				error("command %c not recognized\n", c);
				return false;
		}
	}

	return true;
}
