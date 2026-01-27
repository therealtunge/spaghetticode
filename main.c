#include <stdio.h>
#include <stdlib.h>
#include <vm.h>
#include <string.h>


int main(int argc, char *argv[]) {
	if (argc != 2) {
		printf("wrong amount of args: %d, expected 1\n", argc-1);
		return 1;
	}
	FILE *fp = fopen((const char*)argv[1], "rb");

	if (fp == NULL) {
		perror("open");
	}
	// get file size
	fseek(fp, 0L, SEEK_END); 
	size_t sz = ftell(fp);

	fseek(fp, 0L, SEEK_SET); // rewind
	
	printf("loading file of %ld bytes...\n", sz);

	char *buffer;

	char *vm_rambuffer;

	buffer = malloc(sz + 1);
	vm_rambuffer = malloc(65535);
	fread(buffer, sz, 1, fp);
	// parsing time
	int ram_size = 0;
	if (buffer[0] == 0x89) { // binary
		memcpy(buffer + 1, vm_rambuffer, sz - 1);
		ram_size = sz - 1;
	} else if (buffer[0] == 'x') { // not binary

		int temp;
		char *bufold = buffer;
		buffer++;
		for (int i = 0; *buffer; i+=2) {
			temp = strtoul(buffer, NULL, 16);
			vm_rambuffer[i/2] = temp;
			buffer = strchr(buffer, ' ') + 1;
			if (buffer - bufold > sz) {
				break;
			}
		}
		buffer = bufold; // restore buffer
		ram_size = (sz - 1) / 2;
	} else {
		printf("fish\n");
		goto quit;
	}

	printf("loaded into vm ram: %d bytes\n", ram_size);

	VMstate_t vm;
	
	vm.regs.sp = 32768;	// this might not make any sense, but it makes this pass a test
				// (do enter-method, do return, return pops off 0x00000000,
				// return jumps to begining) instead of parsing code as a
				// return address and getting sent to NOP land

	vm.regs.buffer = malloc(0);
	vm.regs.allocated = 0;

	vm.methods = malloc(sizeof(method_t));
	vm.methodcount = 1;

	vm.ram = vm_rambuffer;
	while (!processInstruction(&vm));
	// clean up...
quit:
	free(vm.regs.buffer);
	free(buffer);
	fclose(fp);
	return 0;
}
