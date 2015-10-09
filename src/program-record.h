#pragma once

#define PAGE_SIZE 1024

struct ProgramPage {
	char data[PAGE_SIZE];
	int curPos = 0;
	ProgramPage *next = NULL;
	ProgramPage *prev = NULL;
};

struct ProgramLocation {
	ProgramPage *page;
	int offset;

	ProgramLocation(ProgramPage *page, int offset)
		: page(page), offset(offset) {}
};
