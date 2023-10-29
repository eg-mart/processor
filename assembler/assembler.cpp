#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <string.h>
#include <ctype.h>
#include <assert.h>

#include "../common/cmd_args.h"
#include "../common/file_reading.h"
#include "../common/common.h"
#include "../common/logger.h"

#define max(a, b) (a) > (b) ? (a) : (b)

const size_t REG_NAME_SIZE		= 64;
const size_t LABEL_SIZE 		= 32;
const size_t MAX_LABELS 		= 64;
const size_t ERR_CONTEXT_SIZE 	= 512;

enum AsmErrorCode {
	ASM_DUPLICATE_LABEL_ERR = -10,
	ASM_TXT_ERR				= -9,
	ASM_OUT_OF_MEMORY		= -8,
	ASM_UNKNOWN_LABEL_ERR	= -7,
	ASM_TOO_MANY_LABELS		= -6,
	ASM_LABEL_TOO_LONG		= -5,
	ASM_FILE_WRITE_ERR  	= -4,
	ASM_WRONG_TOKEN_ERR		= -3,
	ASM_WRONG_ARGS_ERR 		= -2,
	ASM_NO_TOKEN_ERR		= -1,
	ASM_NO_ERR 				=  0,
};

struct AsmError {
	enum AsmErrorCode errcode;
	int padding;
	char context[ERR_CONTEXT_SIZE];
};

struct Label {
	char name[LABEL_SIZE];
	size_t location;
	size_t line;
};

struct Fixup {
	char name[LABEL_SIZE];
	size_t dest;
	size_t line;
};

struct Assembler {
	int *program;
	size_t cmd_num;
	struct Text program_txt;
	struct Label labels[MAX_LABELS];
	struct Fixup fixups[MAX_LABELS];
	size_t labels_num;
	size_t fixups_num;
	size_t pc;
	size_t line;
};

const char *ASM_ERROR_MSG[] = {
	"No errors were encountered\n",
	"Command was expected, but not found: line %d\n",
	"Wrong arguments to a command were found on line %d\n",
	"An unexpected command was encountered on line %d\n",
	"Writing results to a binary file has failed\n",
	"A label is too long on line %d\n",
	"Too many labels in a program. Happened on line %d:\n",
	"An unknown label was encountered: line %d\n",
	"Out of memory\n",
	"Text module: %s\n",
	"A label is set two times in the program (on lines %d and %d):\n",
};

char *skip_space(char *str);
size_t word_len(const char *str);
void skip_after_char(char *str, char chr);

struct AsmError assembler_ctor(struct Assembler *asmb, const char *filename);
struct AsmError assemble_program(struct Assembler *asmb);
struct AsmError read_command(struct Assembler *asmb, char *cmd_start);
struct AsmError read_arguments(struct Assembler *asmb, char *arg_start,
							   int arg_types);
struct AsmError read_label(struct Assembler *asmb, char *label_start);
struct AsmError fixup_labels(struct Assembler *asmb);
struct AsmError write_bytecode(struct Assembler *asmb, const char *filename);

struct AsmError create_error(enum AsmErrorCode code, const void *context);
void log_asm_error(enum Log_level level, struct AsmError err,
				   struct Assembler *asmb);

int main(int argc, const char *argv[])
{
	logger_ctor();
	add_log_handler({stderr, DEBUG, true});

	struct Assembler asmb = {};
	struct AsmError asm_err = create_error(ASM_NO_ERR, "");

	struct Args args = {};
	enum IO_error io_err = handle_arguments(argc, argv, &args);
	if (io_err < 0) {
		log_message(ERROR, cmd_err_to_str(io_err));
		goto error;
	}
	
	asm_err = assembler_ctor(&asmb, args.input_filename);
	if (asm_err.errcode < 0) {
		log_asm_error(ERROR, asm_err, &asmb);
		goto error;
	}

	asm_err = assemble_program(&asmb);
	if (asm_err.errcode < 0) {
		log_asm_error(ERROR, asm_err, &asmb);
		goto error;
	}

	asm_err = fixup_labels(&asmb);
	if (asm_err.errcode < 0) {
		log_asm_error(ERROR, asm_err, &asmb);
		goto error;
	}

	for (size_t i = 0; i < asmb.labels_num; i++)
		log_message(DEBUG, "Label %s: %d (line %d)\n", asmb.labels[i].name,
					asmb.labels[i].location, asmb.labels[i].line + 1);
	
	asm_err = write_bytecode(&asmb, "prog1.obj");
	if (asm_err.errcode < 0) {
		log_asm_error(ERROR, asm_err, &asmb);
		goto error;
	}

	error:
		logger_dtor();
		destroy_text(&asmb.program_txt);
		if (asmb.program)
			free(asmb.program);
		return 1;
	
	destroy_text(&asmb.program_txt);
	free(asmb.program);
	logger_dtor();

	return 0;
}

struct AsmError assembler_ctor(struct Assembler *asmb, const char *filename)
{
	enum TextError txt_err = read_text(&asmb->program_txt, filename);
	if (txt_err < 0) {
		return create_error(ASM_TXT_ERR, txt_error_to_str(txt_err));
	}

	asmb->program = (int*) calloc(asmb->program_txt.num_lines * 2, sizeof(int));
	if (!asmb->program) {
		return create_error(ASM_OUT_OF_MEMORY, "");
	}
	return create_error(ASM_NO_ERR, "");
}

struct AsmError assemble_program(struct Assembler *asmb)
{
	assert(asmb);

	asmb->pc = 0;
	struct AsmError asm_err = create_error(ASM_NO_ERR, "");
	log_message(INFO, "ASSEMBLING START\n");
	for (asmb->line = 0; asmb->line < asmb->program_txt.num_lines; asmb->line++) {
		char *token_start = skip_space(asmb->program_txt.lines[asmb->line].txt);
		skip_after_char(token_start, ';');
		if (*token_start == '\0')
			continue;
		
		asm_err = read_command(asmb, token_start);

		if (asm_err.errcode < 0)
			asm_err = read_label(asmb, token_start);

		if (asm_err.errcode < 0) {
			return asm_err;
		}
	}

	asmb->cmd_num = asmb->pc;
	asmb->pc = 0;
	asmb->line = 0;
	log_message(INFO, "ASSEMBLING END\n");
	return create_error(ASM_NO_ERR, "");
}

char *skip_space(char *str)
{
	while (*str != '\0' && isspace(*str))
		str++;
	return str;
}

void skip_after_char(char *str, char chr)
{
	while (*str != '\0' && *str != chr)
		str++;
	*str = '\0';
}

size_t word_len(const char *str)
{
	size_t len = 0;
	while (str[len] != '\0' && !isspace(str[len]))
		len++;
	return len;
}

struct AsmError read_command(struct Assembler *asmb, char *cmd_start)
{
	size_t cmd_len = word_len(cmd_start);
	if (cmd_len <= 0)
		return create_error(ASM_NO_TOKEN_ERR, "");
	
	struct AsmError asm_err = create_error(ASM_NO_ERR, "");
	bool is_processed = false;

#define DEF_CMD(name, args_num, arg_types, code)							\
	if (strlen(#name) == cmd_len &&											\
		strncasecmp(cmd_start, #name, cmd_len) == 0) {						\
		is_processed = true;												\
		asmb->program[asmb->pc] = (int) CMD_ ## name;						\
		log_message(INFO, "%d (%s)", asmb->program[asmb->pc], #name);		\
		if (args_num > 0) {													\
			char *arg_start = skip_space(cmd_start + cmd_len);				\
			asm_err = read_arguments(asmb, arg_start, arg_types);			\
			if (asm_err.errcode < 0) return asm_err;						\
			log_string(INFO, ", arg: %d", asmb->program[asmb->pc + 1]);		\
			asmb->pc += args_num + 1; /* two args but single ++	*/			\
		} else {															\
			(asmb->pc)++;													\
		}																	\
		log_string(INFO, "\n");												\
	}																		\
	else																	\

	#include "../common/codegen.h"
	{ ; }

#undef DEF_CMD

	if (!is_processed)
		return create_error(ASM_WRONG_TOKEN_ERR, "");
	return create_error(ASM_NO_ERR, "");
}

struct AsmError read_label(struct Assembler *asmb, char *label_start)
{
	size_t label_len = word_len(label_start);
	if (label_len <= 0)
		return create_error(ASM_NO_TOKEN_ERR, &asmb->line);

	if (label_start[label_len - 1] != ':')
		return create_error(ASM_WRONG_TOKEN_ERR, &asmb->line);

	if (label_len >= LABEL_SIZE)
		return create_error(ASM_LABEL_TOO_LONG, &asmb->line);
	
	if (asmb->labels_num >= MAX_LABELS)
		return create_error(ASM_TOO_MANY_LABELS, &asmb->line);

	for (size_t i = 0; i < asmb->labels_num; i++) {
		size_t cmp_len = max(label_len - 1, strlen(asmb->labels[i].name));
		if (strncmp(label_start, asmb->labels[i].name, cmp_len) == 0) {
			struct AsmError err = create_error(ASM_DUPLICATE_LABEL_ERR, &asmb->labels[i].line);
			return err;
		}
	}
	
	strncpy(asmb->labels[asmb->labels_num].name, label_start, label_len - 1);
	asmb->labels[asmb->labels_num].location = asmb->pc;
	asmb->labels[asmb->labels_num].line = asmb->line;
	asmb->labels_num++;

	return create_error(ASM_NO_ERR, "");
}

struct AsmError read_arguments(struct Assembler *asmb, char *arg_start,
							   int arg_types)
{
	assert(asmb);
	assert(arg_start);

	int read_success = 0;

	if ((arg_types & SIG_IMMED) && !read_success) {
		int immed = 0;
		read_success |= sscanf(arg_start, "%d", &immed);
		if (read_success)
			asmb->program[asmb->pc + 1] = immed;
	}

	if ((arg_types & SIG_NUM) && !read_success) {
		double num = NAN;
		read_success |= sscanf(arg_start, "%lf", &num);
		if (read_success)
			asmb->program[asmb->pc + 1] = (int) (num * 100);
	}

#define DEF_REG(name)											\
	if (strncasecmp(arg_start, #name, strlen(#name)) == 0) {	\
		asmb->program[asmb->pc + 1] = REG_ ## name;				\
		reg_len = strlen(#name);								\
		read_success = 1;										\
	}															\
	else														\

	if ((arg_types & SIG_RAM) && !read_success) {
		if (arg_start[0] == '[') {
			int address = 0;
			int address_len = 0;
			arg_start = skip_space(arg_start + 1);
			read_success |= sscanf(arg_start, "%d%n", &address, &address_len);
			if (read_success) {
				arg_start = skip_space(arg_start + address_len);
				if (arg_start[0] != ']')
					return create_error(ASM_WRONG_ARGS_ERR, "");
				if (read_success) {
					asmb->program[asmb->pc] |= SIG_RAM;
					asmb->program[asmb->pc + 1] = address;
				}
			} else {
				size_t reg_len = 0;
				#include "../common/registers.h"
				{ ; }

				if (read_success) {
					arg_start = skip_space(arg_start + reg_len);
					if (arg_start[0] != ']')
						return create_error(ASM_WRONG_ARGS_ERR, "");
					asmb->program[asmb->pc] |= SIG_RAM | SIG_REG;
				}
			}
		}
	}

	if ((arg_types & SIG_REG) && !read_success) {
		size_t reg_len = 0;
		#include "../common/registers.h"
		{ ; }

		if (read_success) {
			asmb->program[asmb->pc] |= SIG_REG;
		}
	}

#undef DEF_REG

	if ((arg_types & SIG_LABEL) && !read_success) {
		char label[LABEL_SIZE] = {};
		read_success |= sscanf(arg_start, "%s", label);
		if (read_success) {
			asmb->program[asmb->pc + 1] = -1;
			for (size_t i = 0; i < asmb->labels_num; i++)
				if (strcmp(asmb->labels[i].name, label) == 0)
					asmb->program[asmb->pc + 1] = (int) asmb->labels[i].location;
			if (asmb->program[asmb->pc + 1] == -1) {
				strcpy(asmb->fixups[asmb->fixups_num].name, label);
				asmb->fixups[asmb->fixups_num].dest = asmb->pc + 1;
				asmb->fixups[asmb->fixups_num].line = asmb->line;
				asmb->fixups_num++;
			}
		}
	}

	if (!read_success)
		return create_error(ASM_WRONG_ARGS_ERR, arg_start);
	return create_error(ASM_NO_ERR, "");
}

struct AsmError fixup_labels(struct Assembler *asmb)
{
	for (size_t i = 0; i < asmb->fixups_num; i++) {
		for (size_t j = 0; j < asmb->labels_num; j++) {
			if (strcmp(asmb->labels[j].name, asmb->fixups[i].name) == 0)
				asmb->program[asmb->fixups[i].dest] = (int) asmb->labels[j].location;
		}
		if (asmb->program[asmb->fixups[i].dest] == -1)
			return create_error(ASM_UNKNOWN_LABEL_ERR, &asmb->fixups[i].line);
	}

	return create_error(ASM_NO_ERR, "");
}

struct AsmError write_bytecode(struct Assembler *asmb, const char *filename)
{
	assert(asmb);
	assert(filename);

	FILE *output = fopen(filename, "w");
	if (!output)
		return create_error(ASM_FILE_WRITE_ERR, "");

	fwrite(asmb->program, sizeof(int), asmb->cmd_num, output);

	if (ferror(output))
		return create_error(ASM_FILE_WRITE_ERR, "");
	
	fclose(output);

	return create_error(ASM_NO_ERR, "");
}

void log_asm_error(enum Log_level level, struct AsmError err,
				   struct Assembler *asmb)
{
	const char *err_txt = ASM_ERROR_MSG[-((int) err.errcode)];
	struct Line line = {};
	size_t line_idx = 0;
	switch (err.errcode) {
		case ASM_NO_TOKEN_ERR:
		case ASM_WRONG_TOKEN_ERR:
		case ASM_LABEL_TOO_LONG:
		case ASM_UNKNOWN_LABEL_ERR:
		case ASM_TOO_MANY_LABELS:
			line_idx = *((size_t*) err.context);
			log_message(level, err_txt, line_idx + 1);
			line = asmb->program_txt.lines[line_idx];
			log_string(level, skip_space(line.txt));
			log_string(level, "\n");
			for (size_t i = 0; i < line.len; i++)
				log_string(level, "^");
			break;
		case ASM_TXT_ERR:
			log_message(level, err_txt, err.context);
			break;
		case ASM_DUPLICATE_LABEL_ERR:
			log_message(level, err_txt, *((size_t*) err.context) + 1,
						asmb->line + 1);
			line = asmb->program_txt.lines[*((size_t*) err.context)];
			log_string(level, skip_space(line.txt));
			log_string(level, "\n");
			for (size_t i = 0; i < line.len; i++)
				log_string(level, "^");
			break;
		case ASM_OUT_OF_MEMORY:
		case ASM_FILE_WRITE_ERR:
		case ASM_WRONG_ARGS_ERR:
		case ASM_NO_ERR:
		default:
			log_message(level, ASM_ERROR_MSG[-((int) err.errcode)]);
			break;
	}
}

struct AsmError create_error(enum AsmErrorCode code, const void *context)
{
	struct AsmError err = { ASM_NO_ERR, 0, "" };
	err.errcode = code;
	strncpy(err.context, (const char*) context, ERR_CONTEXT_SIZE);
	return err;
}