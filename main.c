#include <stdio.h>
#include <stdlib.h>
#include <vm.h>
#include <dexfile.h>
#include <string.h>

int main(int argc, char *argv[]) {
	if (argc != 2) {
		printf("wrong amount of args: %d, expected 1\n", argc-1);
		return 1;
	}
	FILE *fp = fopen((const char*)argv[1], "rb");

	if (fp == NULL) {
		perror("open");
		goto quit;
	}
	// get file size
	fseek(fp, 0L, SEEK_END); 
	size_t sz = ftell(fp);

	fseek(fp, 0L, SEEK_SET); // rewind
	
	printf("loading file of %ld bytes...\n", sz);
	unsigned int read = 0;
	VMstate_t *vm = malloc(sizeof(VMstate_t));

	vm->methods = malloc(0);
	vm->methodcount = 0;
	vm->callstack = malloc(0);
	vm->callstacksize = 0;
	vm->regstack = malloc(0);
	vm->regstacksize = 0;
	vm->ram = loadDexFile(fp, NULL, &read, vm);
	vm->callsp = 0;
	vm->regsp = 0;
	vm->pc = 0;
	while (!processInstruction(vm));
	// clean up...
quit:
	//free(vm.regs.buffer);
	//free(buffer);
	if (fp) {
		fclose(fp);
	}
	// actually nvm
	free(vm);
	return 0;
}
