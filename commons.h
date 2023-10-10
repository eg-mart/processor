#ifndef _PROC_COMMONS_
#define _PROC_COMMONS_

enum CommandCode {
	ADD = 0,
	SUB = 1,
	MUL = 2,
	DIV = 3,
	PUSH = 4,
	OUT = 5,
	SQRT = 6,
	SIN = 7,
	COS = 8,
	HALT = 9,
	IN = 10,
};

enum ArgTypes {
	NONE	= 1 << 0,
	REG 	= 1 << 1,
	IMMED	= 1 << 2,
};

struct Command {
	enum CommandCode opcode;
	const char *str;
	int arg_types;
};

const struct Command COMMANDS[] = {
	{ ADD, 	"add", 	NONE 	},
	{ SUB, 	"sub", 	NONE 	},
	{ MUL, 	"mul", 	NONE 	},
	{ DIV, 	"div", 	NONE 	},
	{ PUSH, "push", IMMED 	},
	{ OUT, 	"out", 	NONE 	},
	{ SQRT, "sqrt", NONE 	},
	{ SIN, 	"sin", 	NONE 	},
	{ COS, 	"cos", 	NONE 	},
	{ HALT, "hlt", 	NONE 	},
	{ IN, 	"in", 	NONE 	},
};
const size_t NUM_COMMANDS = sizeof(COMMANDS) / sizeof(COMMANDS[0]);

#endif /*_PROC_COMMONS_*/
