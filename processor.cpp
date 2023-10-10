#include <stdio.h>
#include <stdlib.h>

#include "file_reading.h"
#include "logger.h"
#include "execution.h"
#include "stack.h"

int print_int(char *arr, int x, size_t n);

int main()
{
	logger_ctor();
	add_log_handler({stderr, DEBUG, true});
	
	struct Text program_txt = {};
	read_text(&program_txt, "prog1.obj");
	int *program = (int*) calloc(2 * program_txt.num_lines, sizeof(int));
	parse_text(program, &program_txt);

	struct Stack stk = {};
	STACK_CTOR(&stk, print_int);
	execute_program(&stk, program, program_txt.num_lines);
	
	free(program);
	destroy_text(&program_txt);
	stack_dtor(&stk);
	
	return 0;
}

int print_int(char *arr, int x, size_t n)
{
	return snprintf(arr, n, "%d", x);
}