#ifndef _EXEC_MODULE_
#define _EXEC_MODULE_

#include "../common/common.h"
#include "../common/stack.h"
#include "../common/file_reading.h"

const size_t RAM_SIZE = 400;

enum ProgError {
	PROG_NO_HALT_ERR = -4,
	PROG_ZERO_DIV_ERR = -3,
	PROG_WRONG_ARGS_ERR = -2,
	PROG_WRONG_CMD_ERR = -1,
	PROG_NO_ERR = 0,
};

struct SPU {
	struct Stack stk;
	int *program;
	size_t program_size;
	int reg[REGS_NUM];
	int ram[RAM_SIZE];
};

void spu_ctor(struct SPU *spu, FILE *program);
enum ProgError execute_program(struct SPU *spu);

#endif
