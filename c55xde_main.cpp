#include <stdio.h>
#include <stdint.h>
#include <string.h>

#include "c55xde.h"

#define ARRAY_SIZE(x)	(sizeof(x) / sizeof((x)[0]))

struct instruction;
typedef struct instruction insn_item_t;

struct instruction_mask;
typedef struct instruction_mask insn_mask_t;

struct instruction_flag;
typedef struct instruction_flag insn_flag_t;

struct instruction_head;
typedef struct instruction_head insn_head_t;

struct instruction_data;
typedef struct instruction_data insn_data_t;

struct instruction {
	insn_item_t	* i_list;

	insn_mask_t	* m_list;
	insn_flag_t	* f_list;

	const char	* syntax;
};

struct instruction_mask {
	uint8_t		f, n, v;	/* from, number, value */
};

struct instruction_flag {
	uint8_t		f, v;		/* from, value */
};

struct instruction_head {
	uint8_t		byte;
	uint8_t		size;
	insn_item_t	insn;
};

struct instruction_data {
	insn_head_t	* head;
	insn_item_t	* insn;

	union {
		uint8_t		opcode;
		uint8_t		stream[8];
		uint64_t	opcode64;
	};

	// TODO: add specific fields

	uint8_t		f_E;
	uint8_t		f_R;
	uint8_t		f_U;
	uint8_t		f_u;
	uint8_t		f_g;

	uint8_t		f_SS;
	uint8_t		f_DD;
	uint8_t		f_ss;
	uint8_t		f_mm;

	uint8_t		f_SHIFTW;
	uint8_t		f_AAAAAAAI;
};

#define LIST_END			{ 0 }

#define INSN_FLAG(a,b)			{ .f = a, .v = C55X_OPCODE_##b }
#define INSN_MASK(a,b,c)		{ .f = a, .n = b, .v = c }

#define INSN_SYNTAX(arg...)		(const char *)#arg

#define f_list_last(x)			!(((x)->f || (x)->v))
#define m_list_last(x)			!(((x)->f || (x)->n || (x)->v))
#define i_list_last(x)			!(((x)->i_list || (x)->m_list || (x)->f_list || (x)->syntax))

// список описателей команд

insn_head_t insn_head_list[] = {
#if 0
	{
		.byte = 0x20,
		.size = 0x01,
		.insn = {
			.i_list = NULL,
			.m_list = NULL,
			.f_list = (insn_flag_t []) {
				INSN_FLAG(0, E),
				LIST_END,
			},
			.syntax = INSN_SYNTAX(NOP),
		},
	},
	{
		.byte = 0x56,
		.size = 0x02,
		.insn = {
			.i_list = (insn_item_t []) {
				{
					.i_list = NULL,
					.m_list = (insn_mask_t []) {
						INSN_MASK(14, 1, 0),
						LIST_END,
					},
					.f_list = NULL,
					.syntax = INSN_SYNTAX(MAC[R] ACx, Tx, ACy[, ACy]),
				},
				{
					.i_list = NULL,
					.m_list = (insn_mask_t []) {
						INSN_MASK(14, 1, 1),
						LIST_END,
					},
					.f_list = NULL,
					.syntax = INSN_SYNTAX(MAC[R] ACy, Tx, ACx, ACy),
				},
				LIST_END,
			},
			.m_list = NULL,
			.f_list = (insn_flag_t []) {
				INSN_FLAG(0, E),
				INSN_FLAG(14, DD),
				INSN_FLAG(12, SS),
				INSN_FLAG(10, ss),
				INSN_FLAG(8, R),
				LIST_END,
			},
			.syntax = NULL,
		},
	},
#else
#  include "c55xde.table.h"
#endif
};

#include <map>

// хеш-таблица для трансляции опкодов

std::map< uint8_t,
	  insn_head_t * > insn_head_hash;

#define get_bits(v, f, n)	(((v) >> (f)) & ((2 << (n - 1)) - 1))

////////////////////////////////////////////////////////////////////////////////

int run_f_list(insn_data_t * data, insn_item_t * insn)
{
	insn_flag_t * flag;

	if (!insn->f_list)
		return 0;

	for (flag = insn->f_list; !f_list_last(flag); flag++) {
		switch (flag->v) {

		/* 1bl parsing */
		case C55X_OPCODE_E:
			data->f_E = get_bits(data->opcode64, flag->f, 1);
			printf("  E = %01x\n", data->f_E);
			break;
		case C55X_OPCODE_R:
			data->f_R = get_bits(data->opcode64, flag->f, 1);
			printf("  R = %01x\n", data->f_R);
			break;
		case C55X_OPCODE_U:
			data->f_U = get_bits(data->opcode64, flag->f, 1);
			printf("  U = %01x\n", data->f_U);
			break;
		case C55X_OPCODE_Y:
			printf("  Y = %01x (FIXME)\n", (int)get_bits(data->opcode64, flag->f, 1));
			break;
		case C55X_OPCODE_u:
			data->f_u = get_bits(data->opcode64, flag->f, 1);
			printf("  u = %01x\n", data->f_R);
			break;
		case C55X_OPCODE_g:
			data->f_g = get_bits(data->opcode64, flag->f, 1);
			printf("  g = %01x\n", data->f_R);
			break;

		/* 2bl parsing */
		case C55X_OPCODE_SS:
			data->f_SS = get_bits(data->opcode64, flag->f, 2);
			printf("  SS = %02x\n", data->f_SS);
			break;
		case C55X_OPCODE_DD:
			data->f_DD = get_bits(data->opcode64, flag->f, 2);
			printf("  DD = %02x\n", data->f_DD);
			break;
		case C55X_OPCODE_ss:
			data->f_ss = get_bits(data->opcode64, flag->f, 2);
			printf("  ss = %02x\n", data->f_ss);
			break;
		case C55X_OPCODE_mm:
			data->f_mm = get_bits(data->opcode64, flag->f, 2);
			printf("  mm = %02x\n", data->f_mm);
			break;

		/* 6bl parsing */
		case C55X_OPCODE_SHIFTW:
			data->f_SHIFTW = get_bits(data->opcode64, flag->f, 6);
			printf("  SHIFTW = %02x\n", data->f_SHIFTW);
			break;

		/* 8bl parsing */
		case C55X_OPCODE_AAAAAAAI:
			data->f_AAAAAAAI = get_bits(data->opcode64, flag->f, 8);
			printf("  AAAAAAAI = %02x\n", data->f_AAAAAAAI);
			break;

		default:
			printf("TODO: unknown opcode flag %02x\n", flag->v);
			break;
		}
	}

	return 0;
}

int run_m_list(insn_data_t * data, insn_item_t * insn)
{
	insn_mask_t * mask;

	if (!insn->m_list)
		return 0;

	for (mask = insn->m_list; !m_list_last(mask); mask++) {
		/* match bits in range [f, f + n] with mask's value */
		if (get_bits(data->opcode64, mask->f, mask->n) != mask->v)
			return -1;
	}

	return 0;
}

int decode_insn(insn_data_t * data)
{
	insn_item_t * insn = &data->head->insn;

	run_f_list(data, insn);

	if (insn->i_list) {
		for (insn = insn->i_list; !i_list_last(insn); insn++) {
			if (!run_m_list(data, insn)) {
				run_f_list(data, insn);
				break;
			}

		}
	}

	printf("insn: %s\n", insn ? insn->syntax : "undef");

	return data->head->size;
}

int lookup_insn(insn_data_t * data)
{
	uint8_t opcode = data->opcode;

	static uint8_t e_list[] = {
		0xF8, 0x60,	/* 0110 0lll */
		0xF0, 0xA0,	/* 1010 FDDD */
		0xFC, 0xB0,	/* 1011 00DD */
		0xF0, 0xC0,	/* 1100 FSSS */
		0xFC, 0xBC,	/* 1011 11SS */
	};

	/* handle some exceptions */

	for (int i = 0; i < ARRAY_SIZE(e_list); i += 2) {
		if ((opcode & e_list[i]) == e_list[i + 1]) {
			data->head = insn_head_hash[e_list[i + 1]];
			break;
		}
	}

	data->head = data->head ? : \
		(insn_head_hash[opcode] ? : insn_head_hash[opcode & 0xFE]);

	return data->head ? 0 : -1;
}

void insn_data_init(insn_data_t * data, const uint8_t * stream)
{
	memset(data, 0, sizeof(*data));
	memcpy(data->stream, stream, sizeof(data->stream));
}

int decode(const uint8_t * stream)
{
	insn_data_t data;

	insn_data_init(&data, stream);

	if (lookup_insn(&data)) {
		printf("unsupported insn: %02x\n", stream[0]);
		return -1;
	}

	return decode_insn(&data);
}

void initialize(void)
{
	for (int i = 0; i < ARRAY_SIZE(insn_head_list); i++) {
		insn_head_hash[ insn_head_list[i].byte ] = &insn_head_list[i];
	}
}

int main(int argc, const char * argv[])
{
	int length;
	uint8_t data[] = { 0x20,			// NOP
			   0x21,			// NOP E
			   0x56, 0xFF,			// MAC[R] ...
			   0xFA, 0x00, 0x00, 0x04,	// MOV [rnd ...
			   0xFD, 0x00, 0x00, 0x00,	// MPY[R] ... :: MPY[R] ...
			   0xFD, 0xff, 0x04, 0x00,	// MPY[R] ... :: MAC[R] ...
			   0xFF }, * p = data;

	initialize();

	while ((length = decode(p)) > 0) {
		p += length;
	}

	return 0;
}
