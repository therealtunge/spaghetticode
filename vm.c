#include <vm.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
uint32_t parse_u32(char *ram, unsigned int ptr) {
	return *((unsigned int*) (ram + ptr));
}

char *parse_pointer(char *ram, unsigned int ptr) {
	int p = *((unsigned int*) (ram + ptr));
	return ram + p;
}

int8_t parse_i8(char *ram, unsigned int ptr) {
	return *((int8_t*) (ram + ptr));
}

uint16_t parse_u16(char *ram, unsigned int ptr) {
	return *((uint16_t*) (ram + ptr));
}

int16_t parse_i16(char *ram, unsigned int ptr) {
	return *((int16_t*) (ram + ptr));
}


void write_u32(char *ram, unsigned int ptr, uint32_t value) {
	*((unsigned int *)(ram + ptr)) = value;
}


void push_u32(VMstate_t *vm, uint32_t value) {
	//printf("%d %d\n", vm->stacksize, vm->sp);
	if (vm->stacksize <= vm->sp) {
		vm->stacksize += 4;
		vm->stack = realloc(vm->stack, vm->stacksize);
	}
	write_u32(vm->stack, vm->sp, value);
	vm->sp += 4;
}

uint32_t pop_u32(VMstate_t *vm) {
	vm->sp -= 4;
	return parse_u32(vm->stack, vm->sp);
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
	printf(" pc = %d\n", vm->pc);
	if (vm->pc == 0) {
		for (int i = 0; i <= vm->methodcount; i++) {

			if (strncmp(vm->methods[i].name, "main", 4) == 0) {
				vm->pc = vm->methods[i].address;
				return 0;
			}
		}
	}
	
	if (vm->pc % 4 == 0) {
		if (is_address_method(vm, vm->pc)) {
			printf("METHOD JUMP!\n");
			
			for (int i = 0; i < parse_u16(vm->ram, vm->pc); i++) {
				push_u32(vm, 0);
			}
			vm->pc += 0x10;
			return 0;
		}
	}
	
	switch (vm->ram[vm->pc]) {
		case 0x00: {
			vm->pc += 2;
			return 0;
		}
		case 0x12: {
			//printf("AAAAAAAAAAAAAAAAAAAAAAAAA\n");
			vm->stack[vm->ram[vm->pc + 1] & 0xf] = vm->ram[vm->pc + 1] & (0xf0 >> 4);
			vm->pc += 2;
			return 0;
		}
		case 0x0f: {
			vm->pc = pop_u32(vm);
			vm->result = vm->stack[(int)vm->ram[vm->pc+1]];
			if (vm->sp == 0) {
				printf("main() returned exit code %d\n", vm->result);
				exit(0);
			}
			printf("ABBBB %d\n", vm->pc);
			return 0;
		}
		case 0x28: {
			vm->pc = parse_i8(vm->ram, vm->pc + 1);
			return 0;
		}
		case 0x70: {

		}
		default: {
			printf("ERROR: unknown instruction at %d: 0x%x\n", vm->pc, vm->ram[vm->pc]);
			return 1;
		}
	}
	return 1;
}
