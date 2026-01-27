#pragma once

typedef struct {
	char *method_name;
	long long *buffer;
	unsigned int allocated;
} RegBank_t;

typedef struct {
	unsigned int pc;
	unsigned long long sp;
	RegBank_t *buffer;
	unsigned int allocated;
} RegsState_t;

typedef struct {
	int locals;
	char *name;
} method_t;

typedef struct {
	RegsState_t regs;
	RegBank_t *cur_regbank;
	method_t *methods;
	int methodcount;
	char *ram;
} VMstate_t;


int processInstruction(VMstate_t *vm);
