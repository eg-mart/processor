#ifndef _PROC_COMMONS_
#define _PROC_COMMONS_

enum ArgTypes {
	NONE	= 1 << 0,
	IMMED	= 1 << 1,
	LABEL	= 1 << 2,
	NUM 	= 1 << 3,
	REG 	= 1 << 5,
	RAM		= 1 << 6,
};

#define DEF_REG(name) \
	REG_ ## name,

enum Registers {
	#include "registers.h"
	REGS_NUM
};

#undef DEF_REG

#define DEF_CMD(name, args_num, arg_types, code) \
	CMD_ ## name,

enum CommandCode {
	#include "codegen.h"
};

#undef DEF_CMD

#endif /*_PROC_COMMONS_*/
