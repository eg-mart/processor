#include "execution.h"
#include "stack_debug.h"
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <float.h>

void add(struct Stack *stk);
void sub(struct Stack *stk);
void mul(struct Stack *stk);
void div(struct Stack *stk);
void push(struct Stack *stk, int arg);
void out(struct Stack *stk);
void sqrt(struct Stack *stk);
void sin(struct Stack *stk);
void cos(struct Stack *stk);
void in(struct Stack *stk);

void parse_text(int program[], struct Text *raw)
{
	int ip = 0;
	for (size_t i = 0; i < raw->num_lines; i++) {
		int opcode = 0;
		double arg = NAN;
		sscanf(raw->lines[i].txt, "%d %lf", &opcode, &arg);
		program[ip++] = opcode;
		if (!isnan(arg))
			program[ip++] = (int) (arg * 100);
	}
}

enum ProgError execute_program(struct Stack *stk, int program[], size_t n)
{
	for (size_t i = 0; i < 2 * n; i++) {
		switch (program[i]) {
			case ADD:
				add(stk);
				break;
			case SUB:
				sub(stk);
				break;
			case MUL:
				mul(stk);
				break;
			case DIV:
				div(stk);
				break;
			case PUSH:
				push(stk, program[++i]);
				break;
			case OUT:
				out(stk);
				break;
			case SQRT:
				sqrt(stk);
				break;
			case SIN:
				sin(stk);
				break;
			case COS:
				cos(stk);
				break;
			case IN:
				in(stk);
				break;
			case HALT:
				return PROG_NO_ERR;
			default:
				return PROG_WRONG_CMD_ERR;
		}
	}
	return PROG_NO_HALT_ERR;
}

void add(struct Stack *stk)
{
	int f = 0;
	int s = 0;
	stack_pop(stk, &f);
	stack_pop(stk, &s);
	stack_push(stk, f + s);
}

void sub(struct Stack *stk)
{
	int f = 0;
	int s = 0;
	stack_pop(stk, &f);
	stack_pop(stk, &s);
	stack_push(stk, s - f);
}

void mul(struct Stack *stk)
{
	int f = 0;
	int s = 0;
	stack_pop(stk, &f);
	stack_pop(stk, &s);
	stack_push(stk, s * f / 100);
}

void div(struct Stack *stk)
{
	int f = 0;
	int s = 0;
	stack_pop(stk, &f);
	stack_pop(stk, &s);
	if (f != 0)
		stack_push(stk, s * 100 / f);
}

void push(struct Stack *stk, int arg)
{
	stack_push(stk, arg);
}

void out(struct Stack *stk)
{
	int f = 0;
	stack_pop(stk, &f);
	printf("%.2lf\n", (double) f / 100);
}

void sqrt(struct Stack *stk)
{
	int f = 0;
	stack_pop(stk, &f);
	int s = (int) (100 * sqrt((double) f / 100));
	stack_push(stk, s);
}

void sin(struct Stack *stk)
{
	int f = 0;
	stack_pop(stk, &f);
	int s = (int) (100 * sin((double) f / 100));
	stack_push(stk, s);
}

void cos(struct Stack *stk)
{
	int f = 0;
	stack_pop(stk, &f);
	int s = (int) (100 * cos((double) f / 100));
	stack_push(stk, s);
}

void in(struct Stack *stk)
{
	double f = 0;
	puts("Enter a number:");
	scanf("%lf", &f);
	stack_push(stk, (int) (f * 100));
}