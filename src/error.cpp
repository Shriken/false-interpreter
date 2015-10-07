#include "error.h"

void error(const char *format, ...) {
	printf("[31;1merror:[0;m ");
	va_list args;
	va_start(args, format);
	vprintf(format, args);
	va_end(args);
}
