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

	header_item_t header;

	fread(&header, sizeof(header_item_t), 1, fp);
	if (header.endian_tag != 0x12345678) {
		printf("good job\n");
		return 1;
	}
	printf("dex v");
	for (int i = 4; i < 8; i++) {
		printf("%c", header.magic[i]);
	}
	printf("\n");
	

//	while (!processInstruction(&vm));
	// clean up...
quit:
	//free(vm.regs.buffer);
	//free(buffer);
	if (fp) {
		fclose(fp);
	}
	
	return 0;
}
