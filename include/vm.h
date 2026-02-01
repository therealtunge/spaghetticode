#pragma once


typedef struct { // bad reimplementation of a map
	char *name;
	unsigned int address;
} method_t;
#define write_register(r, v) vm->regstack[(vm->register_offset + (r))] = v
#define read_register(r) vm->regstack[(vm->register_offset + (r))]
typedef struct {
	unsigned int cur_method_locals;
	method_t *methods;
	unsigned int methodcount;
	unsigned int *callstack;
	unsigned int callstacksize;
	unsigned int *regstack;
	unsigned int regstacksize;
	unsigned char *ram;
	unsigned int callsp;
	unsigned int regsp;
	unsigned int pc;
	unsigned int result;
	unsigned int register_offset;
} VMstate_t;


int processInstruction(VMstate_t *vm);
