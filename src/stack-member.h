#pragma once

enum StackMemberType {
	INTEGER,
	LAMBDA,
	VARIABLE,
	STRING
};

union StackMemberContents {
	int integer;
	char *lambda; // TODO fix lambdas
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
};
