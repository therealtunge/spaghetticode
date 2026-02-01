#include <vm.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
static inline uint32_t parse_u32(void *ram, unsigned int ptr) {
	return *((unsigned int*) (ram + ptr));
}

static inline char *parse_pointer(void *ram, unsigned int ptr) {
	int p = *((unsigned int*) (ram + ptr));
	return ram + p;
}

static inline int8_t parse_i8(void *ram, unsigned int ptr) {
	return *((int8_t*) (ram + ptr));
}

static inline uint16_t parse_u16(void *ram, unsigned int ptr) {
	return *((uint16_t*) (ram + ptr));
}

static inline int16_t parse_i16(void *ram, unsigned int ptr) {
	return *((int16_t*) (ram + ptr));
}


static inline void write_u32(void *ram, unsigned int ptr, uint32_t value) {
//	printf("%d ADBBS ADBSABA\n", value);
	*((unsigned int *)(ram)) = value;
}


void push_u32(VMstate_t *vm, uint32_t value) {

	if (vm->callstacksize <= vm->callsp) {
		vm->callstacksize += 4;
		vm->callstack = realloc(vm->callstack, vm->callstacksize);
	}
	write_u32(vm->callstack, vm->callsp, value);
	
	vm->callsp += 4;
//	printf("ABDASDADBZASVDSDBXDFCZFVDGBGMHGVFNH %d %d %d\n", vm->callstacksize, vm->callsp, value);
}
void push_regu32(VMstate_t *vm, uint32_t value) {
//	printf("reg %d %d %d\n", vm->regstacksize, vm->regsp, value);
	if (vm->regstacksize <= vm->regsp) {
		vm->regstacksize += 4;
		vm->regstack = realloc(vm->regstack, vm->regstacksize);
	}
	write_u32(vm->regstack, vm->regsp, value);
	vm->regsp += 4;
}
uint32_t pop_regu32(VMstate_t *vm) {
	vm->regsp -= 4;
	return parse_u32(vm->regstack, vm->regsp);
}
uint32_t pop_u32(VMstate_t *vm) {
	vm->callsp -= 4;
	//printf("AAA %d %d\n", vm->callstacksize, vm->callsp);
	return parse_u32(vm->callstack, vm->callsp);
}

bool is_address_method(VMstate_t *vm, unsigned int addr) {
	unsigned int i = 0;
	while (vm->methods[i].name) {
		if (vm->methods[i].address == addr) {
			return true;
		}
		i++;
	}
	return false;
}

int processInstruction(VMstate_t *vm) {
	printf("pc = %d\n", vm->pc);
	if (vm->pc == 0) {
		for (int i = 0; i <= vm->methodcount; i++) {

			if (strncmp(vm->methods[i].name, "main", 4) == 0) {
				vm->pc = vm->methods[i].address;
				return 0;
			}
		}
	}
	
	if (true) {
		if (is_address_method(vm, vm->pc)) {
			//printf("METHOD JUMP!\n");
			
			for (int i = 0; i < parse_u16(vm->ram, vm->pc); i++) {
				push_regu32(vm, 0);
			}
			vm->cur_method_locals = parse_u16(vm->ram, vm->pc);
			vm->pc += 0x10;
			return 0;
		}

	}
	
	switch (vm->ram[vm->pc]) {
		case 0x00: {
			printf("AAAAAA\n");
			vm->pc += 2;
			return 0;
		}
		case 0x0a: {
			write_register(vm->ram[vm->pc + 1], vm->result);
			vm->pc += 2;
			return 0;
		}
		case 0x0f: {
			vm->result = vm->regstack[(int)vm->ram[vm->pc+1] + vm->register_offset];

			if (vm->callsp == 0) {
				printf("main() returned exit code %d\n", vm->result);
				exit(vm->result);
			}
			for (int i = -1; i < vm->cur_method_locals; i++) {
				pop_regu32(vm);
			}
			vm->pc = pop_u32(vm);
			
			
			printf("RETURN %d\n", vm->result);
			
			printf("ABBBB %d\n", vm->pc);
			return 0;
		}
		case 0x12: {
			//printf("AAAAAAAAAAAAAAAAAAAAAAAAA\n");
			write_register(vm->ram[vm->pc + 1] & 0xf, vm->ram[vm->pc + 1] & (0xf0 >> 4));
			vm->pc += 2;
			return 0;
		}
		case 0x13: {
			//printf("AAAAAAAAAAAAAAAAAAAAAAAAA\n");
			write_register(vm->ram[vm->pc + 1], parse_u16(vm->ram, vm->pc+2));
			//printf("%x\n", parse_u16(vm->ram, vm->pc+2));
			vm->pc += 4;
			return 0;
		}
		
		case 0x28: {
			vm->pc = parse_i8(vm->ram, vm->pc + 1);
			return 0;
		}
		case 0x71: {
			printf("AaA %d, %d\n", vm->methods[parse_u16(vm->ram, vm->pc + 2)].address, vm->pc + 4);
			push_u32(vm, vm->pc+4);
			vm->pc = vm->methods[parse_u16(vm->ram, vm->pc + 2)].address;
			return 0;
		}
		case 0xd0: {
			write_register(vm->ram[vm->pc + 1] & 0xf, read_register((vm->ram[vm->pc + 1] & 0xf0) >> 4) + parse_u16(vm->ram, vm->pc + 2));
			vm->pc += 4;
			return 0;
		}
		default: {
			printf("ERROR: unknown instruction at %d: 0x%x\n", vm->pc, vm->ram[vm->pc]);
			return 1;
		}
	}
	return 1;
}
