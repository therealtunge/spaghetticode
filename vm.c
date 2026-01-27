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
	write_u32(vm->ram, vm->regs.sp, value);
	vm->regs.sp += 4;
}

uint32_t pop_u32(VMstate_t *vm) {
	vm->regs.sp -= 4;
	return parse_u32(vm->ram, vm->regs.sp);
}

int processInstruction(VMstate_t *vm) {
	switch (vm->ram[vm->regs.pc]) {
		case 0x00: {
			vm->regs.pc++;
			return 0;
		}
		case 0x28: {
			vm->regs.pc = parse_i8(ram, vm->regs.pc + 1);
			return 0;
		}
		default: {
			printf("ERROR: unknown instruction at %d: 0x%x\n", vm->regs.pc, vm->ram[vm->regs.pc]);
			return 1;
		}
	}
}
