#include <stdint.h>

// DEX v37 READER

typedef struct {
	uint8_t magic[8];
	uint32_t checksum; // unused for now :D
	uint8_t sha1[20]; // unused for now :D
	uint32_t filesize; // filesize 
	uint32_t header_size; // header size (0x70)
	uint32_t endian_tag; // only reverse endian will be implemented for now (this will come back to bite me :D)

	uint32_t link_size; // idk dont parse it i guess???
	uint32_t link_off;

	uint32_t map_off; // for some reason the map or whatever is broken in a dex i've seen so no

	uint32_t string_ids_size; // we need to parse this
	uint32_t string_ids_off; // this too

	uint32_t type_ids_size;
	uint32_t type_ids_off;

	uint32_t proto_ids_size; // prototypes
	uint32_t proto_ids_off;

	uint32_t field_ids_size;
	uint32_t field_ids_off;

	uint32_t method_ids_size; // methods
	uint32_t method_ids_off;
	
	uint32_t class_defs_size; // classes
	uint32_t class_defs_off;

	uint32_t data_size; // data section size (used since we are v37)
	uint32_t data_off;

//	uint64_t unused;
} header_item_t;

typedef struct {
	uint32_t class_idx;
	uint32_t access_flags;
	uint32_t superclass_idx;
	uint32_t interfaces_off;
	uint32_t source_file_idx;
	uint32_t annotations_off;
	uint32_t class_data_off;
	uint32_t static_values_off;
} class_def_item_t;

typedef struct {
	uint32_t string_data_off;
} string_id_item_t;

typedef struct {
	uint32_t descriptor_idx;
} type_id_item_t;

typedef struct {
	uint32_t shorty_idx;
	uint32_t return_type_idx;
	uint32_t parameters_off;
} proto_id_item_t;

typedef struct {
	uint16_t class_idx;
	uint16_t type_idx;
	uint32_t name_idx;
} field_id_item_t;

typedef struct {
	uint16_t class_idx;
	uint16_t proto_idx;
	uint32_t name_idx;
} method_id_item_t;

#define call_site_item_t assert(0); int // too lazy to define these lol
#define method_handle_item_t assert(0); int