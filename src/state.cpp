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

bool State::evalChar(const char c) {
	if (evalState == CHAR_CODE) {
		// push this char to the stack
		push(new StackMember(c));
		evalState = STANDARD;
	} else if (evalState == IN_STRING) {
		// keep printing until we hit a close quote
		if (c == '"') {
			evalState = STANDARD;
			printf("\n");
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
	} else if ('a' <= c && c <= 'z') {
		// if we were parsing an int before, push it
		if (evalState == IN_NUMBER) push(intValue);
		evalState = STANDARD;

		auto var = new StackMember(c - 'a');
		var->type = VARIABLE;
		push(var);
	} else { // STANDARD mode
		// if we were parsing an int before, push it
		if (evalState == IN_NUMBER) push(intValue);
		evalState = STANDARD;

		// run command normally
		StackMember *first = NULL;
		StackMember *second = NULL;
		StackMember *third = NULL;
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
				first = pop();
				second = pop();
				assert(first != NULL && second != NULL);
				assert(first->type == INTEGER && second->type == INTEGER);

				if (c == '+')
					push(first->data.integer + second->data.integer);
				else if (c == '-')
					push(second->data.integer - first->data.integer);
				else if (c == '*')
					push(first->data.integer * second->data.integer);
				else if (c == '/')
					push(second->data.integer / first->data.integer);
				else if (c == '&')
					push(first->data.integer & second->data.integer);
				else if (c == '|')
					push(first->data.integer | second->data.integer);
				else if (c == '>') {
					push(-(second->data.integer > first->data.integer));
				} else if (c == '=') {
					push(-(second->data.integer == first->data.integer));
				}
				break;

			// unary commands
			case '_': // negate
			case '~': // NOT
			case '.': // print int
			case ',': // print char
				first = pop();
				assert(first != NULL);
				assert(first->type == INTEGER);

				if (c == '_')
					push(-first->data.integer);
				else if (c == '~')
					push(~first->data.integer);
				else if (c == '.')
					printf("%i\n", first->data.integer);
				else if (c == ',')
					printf("%c\n", first->data.integer);
				break;

			case '$': // dup
			case '%': // drop
				first = pop();
				assert(first != NULL);

				if (c == '$') {
					push(new StackMember(first));
					push(new StackMember(first));
				} else if (c== '%') {
					break; // drop
				}
				break;

			case '\\': // swap
				first = pop();
				second = pop();
				assert(first != NULL && second != NULL);
				push(first);
				push(second);
				first = NULL;
				second = NULL;
				break;

			case '`': // pick nth
				first = pop();
				second = topOfStack;
				assert(second != NULL);
				while (first->data.integer-- > 0) {
					second = second->next;
					assert(second != NULL);
				}
				push(new StackMember(*second));
				second = NULL;
				break;

			case '@': // rot
				first = pop();
				second = topOfStack->next;
				first->next = second->next;
				second->next = first;
				second = NULL;
				first = NULL;
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
				push(programLocation);
				topOfStack->data.lambda->nextCommand();
				lambdaDepth++;
				break;

			case '!': // exec lambda
				first = pop();
				assert(first->type == LAMBDA);
				execLambda(first);
				break;

			case '?':
				first = pop();
				second = pop();
				assert(first->type == LAMBDA);
				assert(second->type == INTEGER);

				// if condition is true, exec lambda
				if (second->data.integer == -1) execLambda(first);
				break;

			case '#':
				first = pop();
				assert(first->type == LAMBDA);
				second = pop();
				assert(second->type == LAMBDA);

				// test the condition
				execLambda(second);
				third = pop();
				assert(third->type == INTEGER);
				while (third->data.integer == -1) {
					execLambda(first);

					// test the condition
					execLambda(second);
					delete third;
					third = pop();
					assert(third->type == INTEGER);
				}
				break;

			case ']':
				// we should only hit these in execLambda. parse error!
				error("unmatched end-of-lambda (])\n");
				break;

			case ':':
				// store into variable
				first = pop();
				second = pop();
				assert(first->type == VARIABLE);

				third = variables + first->data.variable;
				if (third->next != NULL && third->type == LAMBDA) {
					delete third->data.lambda;
				}
				variables[first->data.variable] = *second;
				third->next = ((StackMember *)NULL) + 1;
				if (second->type == LAMBDA) {
					third->data.lambda =
						new ProgramLocation(second->data.lambda);
				}
				third = NULL;
				break;

			case ';':
				// fetch from variable
				first = pop();
				assert(first->type == VARIABLE);
				push(new StackMember(variables + first->data.variable));
				break;

			case '(':
				printStack();
				break;

			// whitespace
			case ' ':
			case '\n':
				break;

			default:
				error("command %c (%i) not recognized\n", c, c);
				return false;
		}

		if (first != NULL) delete first;
		if (second != NULL) delete second;
	}

	// step program forward
	programLocation.nextCommand();

	return true;
}

bool State::execLambda(StackMember *member) {
	assert(member->type == LAMBDA);
	auto lambda = member->data.lambda;

	// push current location to the callstack
	auto curLocation = new ProgramLocation(programLocation);
	curLocation->next = callStack;
	callStack = curLocation;

	programLocation = *lambda;
	char c;
	while (
		(c = programLocation.page->data[programLocation.offset]) != ']' ||
		lambdaDepth > 0
	) {
		evalChar(c);
	}

	// pop from callstack
	programLocation = *callStack;
	callStack = callStack->next;

	return true;
}

void State::push(StackMember *stackMember) {
	stackMember->next = topOfStack;
	topOfStack = stackMember;
}

void State::push(ProgramLocation programPos) {
	push(new StackMember(programPos));
}

StackMember *State::pop() {
	assert(topOfStack != NULL);

	StackMember *popped = topOfStack;
	topOfStack = popped->next;
	return popped;
}

void State::addCommand(const char c) {
	if (currentPage->curPos == PAGE_SIZE) {
		currentPage->next = new ProgramPage();
		currentPage = currentPage->next;
	}

	currentPage->data[currentPage->curPos++] = c;
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

void State::printStack() {
	StackMember *member = topOfStack;
	while (member != NULL) {
		ProgramLocation *lambda;
		switch (member->type) {
			case INTEGER:
				printf("INTEGER: %d\n", member->data.integer);
				break;
			case LAMBDA:
				lambda = member->data.lambda;
				printf(
					"LAMBDA: page %p, offset %d\n",
					lambda->page,
					lambda->offset
				);
				break;
			case VARIABLE:
				printf("VARIABLE: %c\n", member->data.variable + 'a');
				break;
		}

		member = member->next;
	}
}
