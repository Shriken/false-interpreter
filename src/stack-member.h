#pragma once

enum StackMemberType {
	INTEGER,
	LAMBDA,
	STRING
};

union StackMemberContents {
	int integer;
	char *lambda;
	char *string;
};

struct StackMember {
	StackMemberType type;
	StackMemberContents data;
};
