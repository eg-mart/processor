#include <stdio.h>
#include <stdlib.h>

#include "../common/file_reading.h"
#include "../common/logger.h"
#include "../common/stack.h"
#include "execution.h"

int print_int(char *arr, int x, size_t n);
void create_op(enum CommandCode opcode, int arg_types);

int main()
{
	logger_ctor();
	add_log_handler({stderr, DEBUG, true});

	FILE *program_file = fopen("prog1.obj", "rb");
	struct SPU spu = {};
	spu_ctor(&spu, program_file);

	STACK_CTOR(&spu.stk, print_int);
	execute_program(&spu);

	fclose(program_file);
	free(spu.program);
	stack_dtor(&spu.stk);
	
	return 0;
}

int print_int(char *arr, int x, size_t n)
{
	return snprintf(arr, n, "%d", x);
}
