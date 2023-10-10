#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include "file_reading.h"
#include "commons.h"

void assemble_program(int *program, struct Text *program_txt);
void write_bytecode(int *program, const char *filename, size_t n);

int main()
{
	struct Text program_txt = {};
	read_text(&program_txt, "prog1.eg");

	int *program = (int*) calloc(program_txt.num_lines * 2, sizeof(int));
	assemble_program(program, &program_txt);

	write_bytecode(program, "prog1.obj", program_txt.num_lines);

	destroy_text(&program_txt);
	free(program);

	return 0;
}

void assemble_program(int *program, struct Text *program_txt)
{
	const size_t BUFF_SIZE = 64;
	size_t ip = 0;
	for (size_t i = 0; i < program_txt->num_lines; i++) {
		char cmd_str[BUFF_SIZE];
		double arg = NAN;
		sscanf(program_txt->lines[i].txt, "%s %lf", cmd_str, &arg);

		for (size_t j = 0; j < NUM_COMMANDS; j++) {
			if (strcmp(cmd_str, COMMANDS[j].str) == 0) {
				program[ip++] = (int) COMMANDS[j].opcode;
				break;
			}
		}

		if (!isnan(arg))
			program[ip++] = (int) (arg * 100);
	}
}

void write_bytecode(int *program, const char *filename, size_t n)
{
	FILE *output = fopen(filename, "w");
	if (!output)
		return;

	for (size_t i = 0; i < n; i++) {
		fprintf(output, "%d", program[i]);
		if (program[i] == PUSH)
			fprintf(output, " %.2lf", (double) program[++i] / 100);
		fprintf(output, "\n");
	}
	fclose(output);
}