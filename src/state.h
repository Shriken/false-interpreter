#pragma once

#include <stdlib.h>
#include <assert.h>

#include "error.h"
#include "stack-member.h"

class State {
	StackMember *topOfStack;
public:
	State() : topOfStack(NULL) {}

	StackMember *getTopOfStack() { return this->topOfStack; }
	bool eval(char *commands);
};
