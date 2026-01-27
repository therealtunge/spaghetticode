#include <vm.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
method_t *allocate_method(VMstate_t *vm) {
	vm->methodcount++;
	return realloc(vm->methods, vm->methodcount * sizeof(method_t));
}
RegBank_t *allocate_regbank(VMstate_t *vm) {
	vm->regs.allocated++;
	return realloc(vm->regs.buffer, vm->regs.allocated * sizeof(RegBank_t));
	
}

RegBank_t *find_regbank(VMstate_t *vm, char *name) {
	for (int i = 0; i < vm->regs.allocated; i++) {
		if (strcmp(vm->regs.buffer[i].method_name, name) == 0) {
			return &vm->regs.buffer[i];
		}
	}
	return NULL;
}

int is_method_unique(VMstate_t *vm, method_t other) {
	for (int i = 0; i < vm->methodcount; i++) {
		if (vm->methods[i].name == other.name) {
			return 0;
		}
	}
	return 1;
}

uint32_t parse_u32(char *ram, unsigned int ptr) {
	return *((unsigned int*) (ram + ptr));
}

char *parse_pointer(char *ram, unsigned int ptr) {
	int p = *((unsigned int*) (ram + ptr));
	return ram + p;
}

uint16_t parse_u16(char *ram, unsigned int ptr) {
	return *((uint16_t*) (ram + ptr));
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

void nop(VMstate_t *vm) {
	printf("NOP!!!!!! %d\n", vm->regs.pc);
}

int processInstruction(VMstate_t *vm) {
	switch (vm->ram[vm->regs.pc]) {
		case 0x00: {
			nop(vm);
			vm->regs.pc++;
			return 0;
		}
		case 0x01: { // enter_method mSig (x4 pointer), locals (x2 int)
			
			if (vm->cur_regbank != NULL) {
				printf("%lld if this isnt 100 then it worked!!!\n", vm->cur_regbank->buffer[0]);
			}

			method_t temp0;
			temp0.name = parse_pointer(vm->ram, vm->regs.pc + 1);
			temp0.locals = parse_u16(vm->ram, vm->regs.pc + 5);

			vm->regs.pc += 7;

			if (is_method_unique(vm, temp0)) {
				RegBank_t temp1;

				temp1.method_name = temp0.name;
				temp1.buffer = malloc(temp0.locals * sizeof(long long));
				temp1.allocated = temp0.locals;

				vm->methods = allocate_method(vm);
				vm->methods[vm->methodcount - 1] = temp0;

				vm->regs.buffer = allocate_regbank(vm);
				vm->regs.buffer[vm->regs.allocated - 1] = temp1;

				vm->cur_regbank = &temp1; // this will probably come back to bite me wont it
			} else {
				vm->cur_regbank = find_regbank(vm, temp0.name);
			}
			
			return 0;
		}
		case 0x02: { // call (target ptr x4)
			push_u32(vm, vm->regs.pc + 5);
			vm->regs.pc = parse_u32(vm->ram, vm->regs.pc + 1);
			return 0;
		}
		case 0x03: { // ret
			vm->regs.pc = pop_u32(vm);
			return 0;
		}
		default: {
			printf("ERROR: unknown instruction at %d: 0x%x\n", vm->regs.pc, vm->ram[vm->regs.pc]);
			return 1;
		}
	}
}
