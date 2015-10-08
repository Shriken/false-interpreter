#include "state.h"

bool State::eval(const char *program) {
	assert(program != NULL);

	// while the command is not null
	for (char c = *(program++); c != 0; c = *(program++)) {
		if (!evalChar(c)) return false;
	}

	return true;
}

bool State::evalChar(const char c) {
	if (evalState == CHAR_CODE) {
		push(new StackMember(c));
		evalState = STANDARD;
	} else if (evalState == IN_STRING) {
		if (c == '"') {
			evalState = STANDARD;
		} else {
			printf("%c", c);
		}
	} else if ('0' <= c && c <= '9') {
		if (evalState != IN_NUMBER) {
			evalState = IN_NUMBER;
			intValue = 0;
		}

		intValue = intValue * 10 + c - '0';
	} else {
		if (evalState == IN_NUMBER) push(intValue);
		evalState = STANDARD;

		StackMember *top = NULL;
		StackMember *second = NULL;
		switch (c) {
			// binary commands
			case '+':
			case '-':
			case '*':
			case '/':
			case '&':
			case '|':
			case '>':
			case '=':
				top = pop();
				second = pop();
				assert(top != NULL && second != NULL);
				assert(top->type == INTEGER && second->type == INTEGER);

				if (c == '+')
					push(top->data.integer + second->data.integer);
				else if (c == '-')
					push(second->data.integer - top->data.integer);
				else if (c == '*')
					push(top->data.integer * second->data.integer);
				else if (c == '/')
					push(second->data.integer / top->data.integer);
				else if (c == '&')
					push(top->data.integer & second->data.integer);
				else if (c == '|')
					push(top->data.integer | second->data.integer);
				else if (c == '>') {
					push(-(second->data.integer > top->data.integer));
				} else if (c == '=') {
					push(-(second->data.integer == top->data.integer));
				}
				break;

			// unary commands
			case '_':
			case '~':
			case '.':
			case ',':
				top = pop();
				assert(top != NULL);
				assert(top->type == INTEGER);

				if (c == '_')
					push(-top->data.integer);
				else if (c == '~')
					push(~top->data.integer);
				else if (c == '.')
					printf("%i", top->data.integer);
				else if (c == ',')
					printf("%c", top->data.integer);
				break;

			case '$':
			case '%':
				top = pop();
				assert(top != NULL);

				if (c == '$') {
					push(top->data.integer);
					push(top->data.integer);
				} else if (c== '%') {
					break; // drop
				}
				break;

			case '\\':
				top = pop();
				second = pop();
				assert(top != NULL && second != NULL);
				push(top);
				push(second);
				top = NULL;
				second = NULL;
				break;

			case '`':
				// pick nth
				top = pop();
				second = topOfStack;
				assert(second != NULL);
				while (top->data.integer-- > 0) {
					second = second->next;
					assert(second != NULL);
				}
				push(new StackMember(*second));
				second = NULL;
				break;

			case '@':
				// rot
				top = pop();
				second = topOfStack->next;
				top->next = second->next;
				second->next = top;
				second = NULL;
				top = NULL;
				break;

			case '^':
				push(getc(stdin));
				break;

			case '<':
				// flush buffered IO
				// TODO
				break;

			case '\'':
				// char literal
				evalState = CHAR_CODE;
				break;

			case '"':
				// string print
				evalState = IN_STRING;
				break;

			case ' ':
				break;

			default:
				error("command %c not recognized\n", c);
				return false;
		}

		if (top != NULL) delete top;
		if (second != NULL) delete second;
	}

	return true;
}

void State::push(StackMember *stackMember) {
	stackMember->next = topOfStack;
	topOfStack = stackMember;
}

StackMember *State::pop() {
	assert(topOfStack != NULL);

	StackMember *popped = topOfStack;
	topOfStack = popped->next;
	return popped;
}
