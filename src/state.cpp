#include "state.h"

State::~State() {
	while (currentPage != NULL) {
		ProgramPage *page = currentPage;
		currentPage = currentPage->prev;
		delete page;
	}

	while (topOfStack != NULL) {
		StackMember *member = topOfStack;
		topOfStack = topOfStack->next;
		delete member;
	}
}

bool State::eval(const char *program) {
	assert(program != NULL);

	// while the command is not null
	char c;
	while ((c = *(program++))) {
		addCommand(c);
		if (!evalChar(c)) return false;
	}

	return true;
}

bool State::evalChar(const char c) {
	int jumped = 0;

	if (evalState == CHAR_CODE) {
		// push this char to the stack
		push(new StackMember(c));
		evalState = STANDARD;
	} else if (evalState == IN_STRING) {
		// keep printing until we hit a close quote
		if (c == '"') {
			evalState = STANDARD;
		} else {
			printf("%c", c);
		}
	} else if (evalState == IN_COMMENT) {
		if (c == '}') {
			evalState = STANDARD;
		}
	} else if (evalState == IN_LAMBDA) {
		// TODO
		if (c == '[') {
			lambdaDepth++;
		} else if (c == ']' && --lambdaDepth == 0) {
			evalState = STANDARD;
		}
	} else if ('0' <= c && c <= '9') {
		// parse number until we hit the end
		if (evalState != IN_NUMBER) {
			evalState = IN_NUMBER;
			intValue = 0;
		}

		intValue = intValue * 10 + c - '0';
	} else { // STANDARD mode
		// if we were parsing an int before, push it
		if (evalState == IN_NUMBER) push(intValue);
		evalState = STANDARD;

		// run command normally
		StackMember *top = NULL;
		StackMember *second = NULL;
		switch (c) {
			// binary commands
			case '+': // add
			case '-': // subtract
			case '*': // multiply
			case '/': // divide
			case '&': // AND
			case '|': // OR
			case '>': // greater than
			case '=': // equal
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
			case '_': // negate
			case '~': // NOT
			case '.': // print int
			case ',': // print char
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

			case '$': // dup
			case '%': // drop
				top = pop();
				assert(top != NULL);

				if (c == '$') {
					push(top->data.integer);
					push(top->data.integer);
				} else if (c== '%') {
					break; // drop
				}
				break;

			case '\\': // swap
				top = pop();
				second = pop();
				assert(top != NULL && second != NULL);
				push(top);
				push(second);
				top = NULL;
				second = NULL;
				break;

			case '`': // pick nth
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

			case '@': // rot
				top = pop();
				second = topOfStack->next;
				top->next = second->next;
				second->next = top;
				second = NULL;
				top = NULL;
				break;

			case '^': // getchar
				push(getc(stdin));
				break;

			case '<': // flush buffered IO
				// TODO
				break;

			case '\'': // push char literal
				evalState = CHAR_CODE;
				break;

			case '"': // print string
				evalState = IN_STRING;
				break;

			case '[': // push lambda
				evalState = IN_LAMBDA;
				push(refTo(currentPage));
				lambdaDepth++;
				break;

			// whitespace
			case ' ':
			case '\n':
				break;

			default:
				error("command %c not recognized\n", c);
				return false;
		}

		if (top != NULL) delete top;
		if (second != NULL) delete second;
	}

	// if we haven't just jumped, step program location forward
	if (!jumped) {
		programLocation.nextCommand();
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

void State::push(ProgramLocation programPos) {
	StackMember *member = new StackMember();
	member->type = LAMBDA;
	member->data.lambda = programPos.page->data + programPos.offset;
	push(member);
}

void State::addCommand(const char c) {
	if (currentPage->curPos == PAGE_SIZE) {
		currentPage->next = new ProgramPage();
		currentPage = currentPage->next;
	}

	currentPage->data[currentPage->curPos++] = c;
}

ProgramLocation State::refTo(ProgramPage *programPos) {
	return ProgramLocation(programPos, programPos->curPos);
}

void State::printEvalState() {
	switch (evalState) {
		case CHAR_CODE:
			printf("CHAR_CODE\n");
			break;
		case IN_STRING:
			printf("IN_STRING\n");
			break;
		case IN_COMMENT:
			printf("IN_COMMENT\n");
			break;
		case IN_NUMBER:
			printf("IN_NUMBER\n");
			break;
		case STANDARD:
			printf("STANDARD\n");
			break;
		default:
			error("bad eval state\n");
			break;
	}
}
