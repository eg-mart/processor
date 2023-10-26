#define POP(varp) 	stack_pop(&spu->stk, (varp))
#define PUSH(var) 	stack_push(&spu->stk, (var))
#define OPCODE	  	spu->program[i]
#define ARG		  	spu->program[i + 1]
#define REG(num)  	spu->reg[(num)]
#define PC 			i
#define RAM(addr)	spu->ram[(addr)]

DEF_CMD(PUSH, 1, REG | NUM | RAM, 
	{
		if ((OPCODE & REG) && (OPCODE & RAM)) {
			PUSH(RAM(REG(ARG) / 100));
		} else if (OPCODE & REG) {
			PUSH(REG(ARG));
		} else if (OPCODE & RAM) {
			PUSH(RAM(ARG));
		} else {
			PUSH(ARG);
		}
	}
)

DEF_CMD(POP,  1, REG | RAM,
	{
		if ((OPCODE & REG) && (OPCODE & RAM)) {
			POP(&(RAM(REG(ARG) / 100)));
		} else if (OPCODE & REG) {
			POP(&(REG(ARG)));
		} else if (OPCODE & RAM) {
			POP(&(RAM(ARG)));
		}
	}
)

DEF_CMD(ADD,  0, NONE,
	{	
		int f = 0;
		int s = 0;
		POP(&f);
		POP(&s);
		PUSH(f + s);
	}
)

DEF_CMD(SUB,  0, NONE,
	{
		int f = 0;
		int s = 0;
		POP(&f);
		POP(&s);
		PUSH(s - f);
	}
)

DEF_CMD(MUL,  0, NONE,
	{
		int f = 0;
		int s = 0;
		POP(&f);
		POP(&s);
		PUSH(f * s / 100);
	}
)

DEF_CMD(DIV,  0, NONE,
	{
		int f = 0;
		int s = 0;
		POP(&f);
		POP(&s);
		if (f != 0) {
			PUSH(s * 100 / f);
		}
	}
)

DEF_CMD(SQRT, 0, NONE,
	{
		int f = 0;
		POP(&f);
		PUSH((int) (100 * sqrt((double) f / 100)));
	}
)

DEF_CMD(SIN,  0, NONE,
	{
		int f = 0;
		POP(&f);
		PUSH((int) (100 * sin((double) f / 100)));
	}
)

DEF_CMD(COS,  0, NONE,
	{
		int f = 0;
		POP(&f);
		PUSH((int) (100 * cos((double) f / 100)));
	}
)

DEF_CMD(IN,	  0, NONE,
	{
		double f = 0;
		printf("Enter a number: ");
		scanf("%lf", &f);
		PUSH((int) (f * 100));
	}
)

DEF_CMD(OUT,  0, NONE,
	{
		int f = 0;
		POP(&f);
		printf("Result: %.2lf\n", (double) f / 100);
	}
)

DEF_CMD(HLT, 0, NONE,
	{
		return PROG_NO_ERR;
	}
)

DEF_CMD(JMP, 1, LABEL | IMMED,
	{
		PC = (size_t) ARG - 2;
	}
)

DEF_CMD(JB, 1, LABEL | IMMED,
	{
		int f = 0;
		int s = 0;
		POP(&f);
		POP(&s);
		if (s < f)
			PC = (size_t) ARG - 2;
	}
)

DEF_CMD(JA, 1, LABEL | IMMED,
	{
		int f = 0;
		int s = 0;
		POP(&f);
		POP(&s);
		if (s > f)
			PC = (size_t) ARG - 2;
	}
)

DEF_CMD(JBE, 1, LABEL | IMMED,
	{
		int f = 0;
		int s = 0;
		POP(&f);
		POP(&s);
		if (s <= f)
			PC = (size_t) ARG - 2;
	}
)

DEF_CMD(JAE, 1, LABEL | IMMED,
	{
		int f = 0;
		int s = 0;
		POP(&f);
		POP(&s);
		if (s >= f)
			PC = (size_t) ARG - 2;
	}
)

DEF_CMD(JE, 1, LABEL | IMMED,
	{
		int f = 0;
		int s = 0;
		POP(&f);
		POP(&s);
		if (s == f)
			PC = (size_t) ARG - 2;
	}
)

DEF_CMD(JNE, 1, LABEL | IMMED,
	{
		int f = 0;
		int s = 0;
		POP(&f);
		POP(&s);
		if (s != f)
			PC = (size_t) ARG - 2;
	}
)

DEF_CMD(CALL, 1, LABEL | IMMED,
	{
		int f = (int) PC + 2;
		PUSH(f);
		PC = (size_t) ARG - 2;
	}
)

DEF_CMD(RET, 0, NONE,
	{
		int f = 0;
		POP(&f);
		PC = (size_t) (f - 1);
	}
)

DEF_CMD(OUTC, 0, NONE,
	{
		int f = 0;
		POP(&f);
		printf("%c", f / 100);
	}
)

DEF_CMD(OUTD, 0, NONE,
	{
		int f = 0;
		POP(&f);
		printf("%d ", f);
	}
)
