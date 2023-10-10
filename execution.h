#include "stack.h"
#include "file_reading.h"
#include "commons.h"

enum ProgError {
	PROG_NO_HALT_ERR = -4,
	PROG_ZERO_DIV_ERR = -3,
	PROG_WRONG_ARGS_ERR = -2,
	PROG_WRONG_CMD_ERR = -1,
	PROG_NO_ERR = 0,
};

void parse_text(int program[], struct Text *raw);
enum ProgError execute_program(struct Stack *stk, int program[], size_t n);
