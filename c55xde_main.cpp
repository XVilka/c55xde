#include <stdio.h>
#include <stdint.h>

#include "c55xde.h"

#if 0
uint32_t get_insn_len(uint8_t opcode)
{
	return get_insn_len_fast(opcode);
}

uint32_t get_insn_len_fast(uint8_t opcode)
{
	//
	// 1-bytes
	//

	switch (opcode) {
	case 0x20:	// 0010 0000	NOP
	case 0x21:	// 0010 0001	NOP +E
	case 0x98:	// 1001 1000	mmap
	case 0x99:	// 1001 1001	port(Smem)
	case 0x9A:	// 1001 1010	port(Smem)
	case 0x9C:	// 1001 1100	<instruction>.LR
	case 0x9D:	// 1001 1101	<instruction>.CR
		return 1;
	}

	//
	// 2-bytes
	//

	switch (opcode) {
	case 0x90:	// 1001 0000	MOV xsrc, xdst
	case 0x91:	// 1001 0001	B ACx
	case 0x92:	// 1001 0010	CALL ACx
	case 0x94:	// 1001 0100	RESET
	case 0x95:	// 1001 0101	INTR k5 / TRAP k5
	case 0x96:	// 1001 0110	XCC / XCCPART
		return 2;
	}

	// 0010 001E ... 0110 0lll
	if (opcode >= 0x22 && opcode <= 0x67)
		return 2;

	// 1001 1110 ... 1100 llll
	if (opcode >= 0x9E && opcode <= 0xCF)
		return 2;

	//
	// 3-bytes
	//

	// TODO: 3, 4, 5
}
#endif

#define ARRAY_SIZE(x)	(sizeof(x) / sizeof((x)[0]))


#include <map>
#include <iostream>

typedef struct {
	int		length;
	const char	* string;
	uint8_t		* bytecode;
} c55x_insn_t;

typedef struct {
	uint8_t		s_e;
	uint8_t		s_r;
} c55x_state_t;

std::map<uint8_t, c55x_insn_t *> c55x_map;

int c55x_decode_one(c55x_insn_t * insn, c55x_state_t * state)
{
	uint8_t * c = insn->bytecode;

	for (uint8_t * c = insn->bytecode; c[0]; c += 2) {

		printf("bytecode %02x %02x\n", c[0], c[1]);

		switch (c[0]) {
		case C55X_OPCODE_E:
			printf("TODO: C55X_OPCODE_E\n");
			break;
		case C55X_OPCODE_R:
			printf("TODO: C55X_OPCODE_R\n");
			break;
		case C55X_OPCODE_AAAAAAAI:
			printf("TODO: C55X_OPCODE_AAAAAAAI\n");
			break;
		default:
			printf("unsupported bytecode: %02x\n", c[0]);
			break;
		}
	}

	return 0;
}


c55x_insn_t * lookup(uint8_t opcode)
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
			return c55x_map[exceptions[i + 1]];
	}

	return c55x_map[opcode] ? : c55x_map[opcode & 0xFE];
}

int decode(const uint8_t * p)
{
	c55x_insn_t * insn;
	c55x_state_t state;

	insn = lookup(p[0]);
	if (!insn) {
		printf("unsupported opcode: %02x\n", p[0]);
		return -1;
	}

	printf("insn = %s\n", insn->string);

	c55x_decode_one(insn, &state);

	return insn->length;
}

typedef struct {
	uint8_t		key;
	uint8_t		* flags;

	const char	* string;
} c55x_table_t;


c55x_table_t c55x_table[] = {
	{
		.key = 0x20,
		.flags = (uint8_t []){ C55X_OPCODE_E, 7,
				       0x00 },
		.string = "NOP",
	},
	{
		.key = 0x56,
		.flags = (uint8_t []){ C55X_OPCODE_E, 7,
				       C55X_OPCODE_DD, 8,
				       C55X_OPCODE_SS, 10,
				       C55X_OPCODE_ss, 12,
				       C55X_OPCODE_R, 15,
				       0x00 },
		.string = "MAC[R] ACx, Tx, ACy[, ACy]",
	},
};


void initialize(void)
{
	for (int i = 0; i < ARRAY_SIZE(c55x_table); i++) {
#if 1
		c55x_insn_t * insn = new c55x_insn_t;

		insn->length = 1;
		insn->string = c55x_table[i].string;
		insn->bytecode = c55x_table[i].flags;

		c55x_map[c55x_table[i].key] = insn;
#else
		c55x_map[c55x_table[i].key] = c55x_table[i];
#endif
	}
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
