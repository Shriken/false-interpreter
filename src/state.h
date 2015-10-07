#pragma once

#include <stdlib.h>
#include <assert.h>

#include "error.h"
#include "stack-member.h"

class State {
	StackMember *topOfStack;
	int stackDepth;
public:
	State() : topOfStack(NULL), stackDepth(0) {}

	StackMember *getTopOfStack() { return this->topOfStack; }
	bool parse(char *commands);
};
