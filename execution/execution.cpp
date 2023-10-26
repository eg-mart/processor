#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <float.h>

#include "execution.h"
#include "../common/common.h"
#include "../common/file_reading.h"
#include "../common/stack_debug.h"
#include "../common/logger.h"

void spu_ctor(struct SPU *spu, FILE *program)
{
	get_file_size(program, &spu->program_size);
	spu->program_size /= sizeof(int);
	spu->program = (int*) calloc(spu->program_size, sizeof(int));
	fread(spu->program, sizeof(int), spu->program_size, program);
}

#define DEF_CMD(name, args_num, arg_types, code)	\
	case CMD_ ## name:								\
		code										\
		i += args_num;								\
		break;

enum ProgError execute_program(struct SPU *spu)
{
	for (size_t i = 0; i < spu->program_size; i++) {
		switch (spu->program[i] & 0x1F) {
			#include "../common/codegen.h"
			default:
				return PROG_WRONG_CMD_ERR;
		}
	}
	
	return PROG_NO_HALT_ERR;
}

#undef DEF_CMD
