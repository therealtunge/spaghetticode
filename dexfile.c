#include <dexfile.h>
#include <stdio.h>
#include <stdlib.h>

#include <string.h>

static inline int readuLEB128(FILE *fp) {
	int result = 0;
	int shift = 0;
	unsigned char byte;
	do {
		fread(&byte, 1, 1, fp);
		result |= (byte & 0x7f) << shift; /* low-order 7 bits of value */
		shift += 7;
	} while ((byte & 0x80) != 0); /* get high-order bit of byte */
	return result;
}

static void loadClassData(FILE *fp, class_data_item_t *cd) {
	cd->static_fields_size = readuLEB128(fp);
	cd->instance_fields_size = readuLEB128(fp);
	cd->direct_methods_size = readuLEB128(fp);
	cd->virtual_methods_size = readuLEB128(fp);
}

static void loadEncodedMethod(FILE *fp, encoded_method_t *em) {
	em->method_idx_diff = readuLEB128(fp);
	em->access_flags = readuLEB128(fp);
	em->code_off = readuLEB128(fp);
}

static char *getString(FILE *fp, unsigned int id, header_item_t header, unsigned int *_len) {
	
	uint32_t string_id;
	unsigned int len;
	
	unsigned int orig = ftell(fp);

	fseek(fp, header.string_ids_off + (id * 4), SEEK_SET);
	fread(&string_id, 4, 1, fp);
	fseek(fp, string_id, SEEK_SET);

	len = readuLEB128(fp);
	char *string;
	string = malloc(len);
	string[len] = 0;
	fread(string, len, 1, fp);
	fseek(fp, orig, SEEK_SET);
	if (_len) {
		*_len = len;
	}

	return string;
}

unsigned char *loadDexFile(FILE *fp, bool *okay, unsigned int *read, VMstate_t *vm) {
	header_item_t header;
	class_def_item_t class;
	class_data_item_t class_data;
	method_id_item_t *methods;
	unsigned char *ram = malloc(0);
	unsigned int ram_size = 0;
	unsigned int ram_cur = 0;
	fread(&header, sizeof(header_item_t), 1, fp);

	methods = malloc(header.method_ids_size * sizeof(method_id_item_t));
	fseek(fp, header.method_ids_off, SEEK_SET);
	fread(methods, sizeof(method_id_item_t), header.method_ids_size, fp);

	fseek(fp, header.class_defs_off, SEEK_SET);
	fread(&class, sizeof(class_def_item_t), 1, fp);
	

	uint32_t string_id;
	fseek(fp, header.type_ids_off + (sizeof(type_id_item_t) * class.class_idx), SEEK_SET);
	fread(&string_id, 4, 1, fp);
	char *string = getString(fp, string_id, header, NULL);
	printf("loading class %s\n", string);
	free(string);
	fseek(fp, class.class_data_off, SEEK_SET);
	loadClassData(fp, &class_data);
	class_data.static_fields = calloc(class_data.static_fields_size, sizeof(encoded_field_t));
	fread(class_data.static_fields, class_data.static_fields_size, sizeof(encoded_field_t), fp);
	class_data.instance_fields = calloc(class_data.instance_fields_size, sizeof(encoded_field_t));
	fread(class_data.instance_fields, class_data.instance_fields_size, sizeof(encoded_field_t), fp);

	class_data.direct_methods = calloc(class_data.direct_methods_size, sizeof(encoded_method_t));
	unsigned int cur_method = 0;
	code_item_t code_item;
	unsigned int len;
	printf("loading %d direct methods\n", class_data.direct_methods_size);
	for (int i = 0; i < class_data.direct_methods_size; i++) {
		loadEncodedMethod(fp, &class_data.direct_methods[i]);
		unsigned int orig = ftell(fp);
		fseek(fp, class_data.direct_methods[i].code_off, SEEK_SET);
		fread(&code_item, sizeof(code_item_t), 1, fp);
		ram_size += (code_item.insns_size * 2) + sizeof(code_item_t);
		ram = realloc(ram, ram_size);
		//memcpy(ram_cur + ram, &code_item, sizeof(code_item_t));
		fseek(fp, class_data.direct_methods[i].code_off, SEEK_SET);
		fread(ram_cur + ram, sizeof(code_item_t), 1, fp);
		
		fread(ram + ram_cur + sizeof(code_item_t), 2, code_item.insns_size, fp);
		fseek(fp, orig, SEEK_SET);
		
		cur_method += class_data.direct_methods[i].method_idx_diff;
		
		vm->methods = realloc(vm->methods, (vm->methodcount + 1) * sizeof(method_t));
		
		string = getString(fp, methods[cur_method].name_idx, header, &len);


		vm->methods[vm->methodcount].name = strdup(string);
		vm->methods[vm->methodcount].address = ram_cur;
		vm->methodcount++;
		printf("loaded direct method %d \"%s\"\n", cur_method, string);
		ram_cur += sizeof(code_item_t) + (code_item.insns_size * 2);
		free(string);
	}
	
	fread(class_data.direct_methods, class_data.direct_methods_size, 1, fp);
	
	class_data.virtual_methods = calloc(class_data.virtual_methods_size, sizeof(encoded_method_t));
	printf("loading %d virtual methods\n", class_data.virtual_methods_size);
	for (int i = 0; i < class_data.virtual_methods_size; i++) {
		loadEncodedMethod(fp, &class_data.virtual_methods[i]);
		cur_method += class_data.virtual_methods[i].method_idx_diff;
		
		string = getString(fp, methods[cur_method].name_idx, header, NULL);

		printf("loaded direct method %d \"%s\"\n", cur_method, string);
	}

	free(methods);
	free(class_data.static_fields);
	free(class_data.instance_fields);
	free(class_data.direct_methods);
	free(class_data.virtual_methods);
	*read = ram_cur;
	return ram;
}