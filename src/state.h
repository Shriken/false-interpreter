#pragma once

#include <assert.h>
#include <stdlib.h>
#include <string.h>

#include "error.h"
#include "program-record.h"
#include "stack-member.h"

enum EvalState {
	CHAR_CODE, // push next char to stack
	IN_STRING, // printing a string
	IN_COMMENT, // in a comment
	IN_LAMBDA,
	IN_NUMBER,
	STANDARD
};

class State {
	int lambdaDepth = 0;
	EvalState evalState = STANDARD;
	StackMember *topOfStack = NULL;
	ProgramPage *currentPage = new ProgramPage();
	ProgramLocation programLocation = ProgramLocation(currentPage, 0);

	ProgramLocation *callStack = NULL;
	StackMember variables[26];

	int intValue;
public:
	State() {
		memset(variables, 0, sizeof(StackMember) * 26);
	}
	~State();

	StackMember *getTopOfStack() { return this->topOfStack; }
	bool evalChar(const char c);
	bool execLambda(StackMember *lambda);

	void push(int val) { push(new StackMember(val)); }
	void push(StackMember *stackMember);
	void push(ProgramLocation programPos);
	StackMember *pop();

	void addCommand(const char c);

	void printEvalState();
	void printStack();
};
