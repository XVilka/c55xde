#include <stdio.h>
#include <stdint.h>

#include "c55xde.h"

#define ARRAY_SIZE(x)	(sizeof(x) / sizeof((x)[0]))

////////////////////////////////////////////////////////////////////////////////

#include <map>

typedef struct {
	uint8_t		type;
	uint8_t		addr;
} insn_flag_t;

typedef struct {
	uint8_t		f, t, v;
} insn_mask_t;

typedef struct {
	insn_mask_t	* masks;
	insn_flag_t	* flags;

	const char	* string;
} insn_item_t;

typedef struct {
	uint8_t		key;
	uint8_t		length;

	insn_flag_t	* flags;
	insn_item_t	* items;
} insn_head_t;

////////////////////////////////////////////////////////////////////////////////

std::map< uint8_t,
	  insn_head_t * > insn_head_map;

insn_head_t heads[] = {
	{
		.key = 0x20,
		.length = 0x01,

		.flags = (insn_flag_t []){
			{ C55X_OPCODE_E, 7 },
			{ 0 },
		},

		.items = (insn_item_t []){
			{
				.masks = 0,
				.flags = 0,
				.string = "NOP",
			},
			{ 0 },
		},
	},
	{
		.key = 0x56,
		.length = 0x02,

		.flags = (insn_flag_t []){
			{ C55X_OPCODE_E, 7 },
			{ C55X_OPCODE_DD, 8 },
			{ C55X_OPCODE_SS, 10 },
			{ C55X_OPCODE_ss, 12 },
			{ C55X_OPCODE_R, 15 },
			{ 0 },
		},

		.items = (insn_item_t []){
			{
				/* 0101011E DDSSss0% */
				.masks = (insn_mask_t []) {
					{ 14, 14, 0 },
				},
				.flags = 0,
				.string = "MAC[R] ACx, Tx, ACy[, ACy]",
			},
			{
				/* 0101100E DDSSss1% */
				.masks = (insn_mask_t []) {
					{ 14, 14, 1 },
				},
				.flags = 0,
				.string = "MAC[R] ACy, Tx, ACx, ACy",
			},
			{ 0 }
		}
	},
};

insn_head_t * lookup_head(uint8_t opcode)
{
	static uint8_t exceptions[] = {
		0xF8, 0x60,	/* 0110 0lll */
		0xF0, 0xA0,	/* 1010 FDDD */
		0xFC, 0xB0,	/* 1011 00DD */
		0xF0, 0xC0,	/* 1100 FSSS */
		0xFC, 0xBC,	/* 1011 11SS */
	};

	for (int i = 0; i < ARRAY_SIZE(exceptions); i += 2) {
		if ((opcode & exceptions[i]) == exceptions[i + 1])
			return insn_head_map[exceptions[i + 1]];
	}

	return insn_head_map[opcode] ? : insn_head_map[opcode & 0xFE];
}

insn_item_t * lookup_item(insn_head_t * head, uint8_t * stream)
{
	insn_item_t * item;

	for (item = head->items; item && item->string; item++) {
		insn_mask_t * mask;

		if (!item->masks)
			break;

		for (mask = item->masks; mask; mask++) {
			printf("TODO: handle mask\n");
		}
	}

	return item;
}

void decode_insn_flags(insn_flag_t * flags, uint8_t * stream)
{
	insn_flag_t * flag;

	for (flag = flags; flag && flag->type != 0; flag++) {
		switch (flag->type) {
		case C55X_OPCODE_E:
			printf("TODO: C55X_OPCODE_E\n");
			break;
		default:
			printf("unsupported flag type: %02x\n", flag->type);
			break;
		}
	}
}

void decode_item_flags(insn_item_t * item, uint8_t * stream)
{
	decode_insn_flags(item->flags, stream);
}

void decode_head_flags(insn_head_t * head, uint8_t * stream)
{
	decode_insn_flags(head->flags, stream);
}

int decode_head(insn_head_t * head, uint8_t * stream)
{
	insn_item_t * item;

	item = lookup_item(head, stream);
	if (item) {
		decode_head_flags(head, stream);
		decode_item_flags(item, stream);

		printf("item %s\n", item->string);
	}

	return item ? head->length : -1;
}

int decode(uint8_t * stream)
{
	insn_head_t * head;

	head = lookup_head(stream[0]);
	if (!head) {
		printf("unsupported insn: %02x\n", stream[0]);
		return -1;
	}

	return decode_head(head, stream);
}

void initialize(void)
{
	for (int i = 0; i < ARRAY_SIZE(heads); i++)
		insn_head_map[ heads[i].key ] = &heads[i];
}

int main(int argc, const char * argv[])
{
	int length;
	uint8_t data[] = { 0x20, 0x21, 0xFF }, * p = data;

	initialize();

	while ((length = decode(p)) > 0) {
		p += length;
	}

	return 0;
}
