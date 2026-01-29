#pragma once

typedef struct {
	unsigned int sp;
	unsigned int pc;
} RegState_t;

typedef struct {
	RegState_t regs;
	char *stack;
	char *ram;
} VMstate_t;


int processInstruction(VMstate_t *vm);
