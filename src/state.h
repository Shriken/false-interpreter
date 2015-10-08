#pragma once

#include <stdlib.h>
#include <assert.h>

#include "error.h"
#include "stack-member.h"

enum EvalState {
	CHAR_CODE, // push next char to stack
	IN_STRING, // printing a string
	IN_COMMENT, // in a comment
	IN_NUMBER,
	STANDARD
};

class State {
	StackMember *topOfStack = NULL;
	EvalState evalState = STANDARD;
	int intValue;
public:
	State() {}

	StackMember *getTopOfStack() { return this->topOfStack; }
	bool eval(const char *commands);
	bool evalChar(const char c);
	void push(int val) { push(new StackMember(val)); }
	void push(StackMember *stackMember);
	StackMember *pop();
};
