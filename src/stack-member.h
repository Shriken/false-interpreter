#pragma once

enum StackMemberType {
	INTEGER,
	LAMBDA,
	VARIABLE,
	STRING
};

union StackMemberContents {
	int integer;
	ProgramLocation *lambda;
	char variable;
	char *string;
};

struct StackMember {
	StackMemberType type;
	StackMemberContents data;
	StackMember *next;

	StackMember() {}
	StackMember(int i) {
		type = INTEGER;
		data.integer = i;
	}
	StackMember(ProgramLocation pos) {
		type = LAMBDA;
		data.lambda = new ProgramLocation(pos);
	}
	~StackMember() {
		if (type == LAMBDA) delete data.lambda;
	}
};
