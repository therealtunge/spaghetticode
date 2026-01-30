#pragma once


typedef struct { // bad reimplementation of a map
	char *name;
	unsigned int address;
} method_t;

typedef struct {
	method_t *methods;
	unsigned int methodcount;
	char *stack;
	char *ram;
	unsigned int sp;
	unsigned int pc;
	unsigned int stacksize;
	unsigned int result;
} VMstate_t;


int processInstruction(VMstate_t *vm);
