#pragma once

enum StackMemberType {
	INTEGER,
	LAMBDA,
	VARIABLE,
};

union StackMemberContents {
	int integer;
	ProgramLocation *lambda;
	int variable;
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

	StackMember(StackMember *member) {
		this->type = member->type;
		if (type == LAMBDA) {
			this->data.lambda = new ProgramLocation(*member->data.lambda);
		} else {
			this->data = member->data;
		}
	}

	~StackMember() {
		if (type == LAMBDA) delete data.lambda;
	}
};
