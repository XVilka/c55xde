#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdarg.h>

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

	struct {
		uint8_t		E;
		uint8_t		R;
		uint8_t		U;
		uint8_t		u;
		uint8_t		g;

		uint8_t		SS;
		uint8_t		DD;
		uint8_t		ss;
		uint8_t		dd;
		uint8_t		mm;
		uint8_t		cc;
		uint8_t		vv;
		uint8_t		tt;

		uint8_t		SHFT;
		uint8_t		SHIFTW;
		uint16_t	AAAAAAAI;

		uint8_t		k3;
		uint8_t		k4;
		uint8_t		k5;
		uint8_t		k6;
		uint8_t		k8;
		uint16_t	k16;
	} f;
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

#define F_INVAL			-1
#define f_valid(x)		((x) != (typeof(x))F_INVAL)

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
			data->f.E = get_bits(data->opcode64, flag->f, 1);
			break;
		case C55X_OPCODE_R:
			data->f.R = get_bits(data->opcode64, flag->f, 1);
			break;
		case C55X_OPCODE_U:
			data->f.U = get_bits(data->opcode64, flag->f, 1);
			break;
		case C55X_OPCODE_Y:
			printf("  Y = %01x (FIXME)\n", (int)get_bits(data->opcode64, flag->f, 1));
			break;
		case C55X_OPCODE_u:
			data->f.u = get_bits(data->opcode64, flag->f, 1);
			break;
		case C55X_OPCODE_g:
			data->f.g = get_bits(data->opcode64, flag->f, 1);
			break;

		/* 2bl parsing */
		case C55X_OPCODE_SS:
			data->f.SS = get_bits(data->opcode64, flag->f, 2);
			break;
		case C55X_OPCODE_DD:
			data->f.DD = get_bits(data->opcode64, flag->f, 2);
			break;
		case C55X_OPCODE_ss:
			data->f.ss = get_bits(data->opcode64, flag->f, 2);
			break;
		case C55X_OPCODE_dd:
			data->f.dd = get_bits(data->opcode64, flag->f, 2);
			break;
		case C55X_OPCODE_mm:
			data->f.mm = get_bits(data->opcode64, flag->f, 2);
			break;
		case C55X_OPCODE_cc:
			data->f.cc = get_bits(data->opcode64, flag->f, 2);
			break;
		case C55X_OPCODE_vv:
			data->f.vv = get_bits(data->opcode64, flag->f, 2);
			break;
		case C55X_OPCODE_tt:
			data->f.tt = get_bits(data->opcode64, flag->f, 2);
			break;

		/* 6bl parsing */
		case C55X_OPCODE_SHIFTW:
			data->f.SHIFTW = get_bits(data->opcode64, flag->f, 6);
			break;

		/* 8bl parsing */
		case C55X_OPCODE_AAAAAAAI:
			data->f.AAAAAAAI = get_bits(data->opcode64, flag->f, 8);
			break;

		case C55X_OPCODE_k3:
			data->f.k3 = get_bits(data->opcode64, flag->f, 3);
			break;
		case C55X_OPCODE_k4:
			data->f.k4 = get_bits(data->opcode64, flag->f, 4);
			break;
		case C55X_OPCODE_k5:
			data->f.k5 = get_bits(data->opcode64, flag->f, 5);
			break;
		case C55X_OPCODE_k6:
			data->f.k6 = get_bits(data->opcode64, flag->f, 6);
			break;
		case C55X_OPCODE_k8:
			data->f.k8 = get_bits(data->opcode64, flag->f, 8);
			break;
		case C55X_OPCODE_k16:
			data->f.k16 = get_bits(data->opcode64, flag->f, 16);
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

void substitute(char * string, const char * token, const char * fmt, ...)
{
	va_list args;
	char data[64];
	char * pos;

	for (;;) {
		pos = strstr(string, token);
		if (!pos)
			return;

		va_start(args, fmt);
		vsprintf(data, fmt, args);
		va_end(args);

		memmove(pos + strlen(data), pos + strlen(token), strlen(pos + strlen(token)) + 1);
		memmove(pos, data, strlen(data));
	}
}

static const char * tbl_RELOP[] = { "==", "<", ">=", "!=" };
static const char * tbl_v[] = { "CARRY", "TC2" };
static const char * tbl_t[] = { "TC1", "TC2" };

void decode_insn_syntax(insn_data_t * data, insn_item_t * insn)
{
	char syntax[1024];

	strcpy(syntax, insn->syntax);

	/* constants */

	substitute(syntax,  "k4", "#%02Xh", data->f.k4);
	substitute(syntax,  "k5", "#%02Xh", data->f.k5);
	substitute(syntax,  "k7", "#%02Xh", data->f.k4 | (data->f.k3 << 4));
	substitute(syntax,  "k8", "#%02Xh", data->f.k8);
	substitute(syntax,  "k9", "#%02Xh", data->f.k4 | (data->f.k5 << 5));
	substitute(syntax, "k16", "#%02Xh", data->f.k16);

	/* RELOP */

	if (f_valid(data->f.cc))
		substitute(syntax, "RELOP", tbl_RELOP[data->f.cc & 3]);

	/* [U], [R], [40] */

	if (f_valid(data->f.R))
		substitute(syntax, "[R]", "%s", data->f.R ? "R" : "");
	if (f_valid(data->f.u))
		substitute(syntax, "[U]", "%s", data->f.u ? "U" : "");
	if (f_valid(data->f.g))
		substitute(syntax, "[40]", "%s", data->f.g ? "40" : "");

	/* Tx */

	if (f_valid(data->f.ss))
		substitute(syntax, "Tx", "T%d", data->f.ss);
	if (f_valid(data->f.dd))
		substitute(syntax, "Tx", "T%d", data->f.dd);

	/* SHFT and SHIFTW */

	if (f_valid(data->f.SHFT))
		substitute(syntax, "SHFT", "%02Xh", data->f.SHFT);

	if (f_valid(data->f.SHIFTW))
		substitute(syntax, "SHIFTW", "%02Xh", data->f.SHIFTW);

	/* vv */

	if (f_valid(data->f.vv)) {
		substitute(syntax, "BitIn", tbl_v[(data->f.vv >> 1) & 1]);
		substitute(syntax, "BitOut", tbl_v[(data->f.vv >> 0) & 1]);
	}

	/* tt */

	if (f_valid(data->f.tt)) {
		substitute(syntax, "TCx", tbl_t[(data->f.tt >> 1) & 1]);
		substitute(syntax, "TCy", tbl_t[(data->f.tt >> 0) & 1]);
	}

	printf("%s\n", syntax);
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

	decode_insn_syntax(data, insn);

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
	memset(&data->f, F_INVAL, sizeof(data->f));
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
			   0x16, 0x07, 0xF0,		// MOV #7Fh, DPH
			   0x00, 0x00, 0xFF,		// RPTCC #FFh, cond
			   0x12, 0x00, 0x00,		// CMP[U] src RELOP dst, TCx
			   0x12, 0x03, 0x01,		// ROL TC2, src, CARRY, dst
			   0x12, 0x03, 0x09,		// ROR CARRY, src, TC2, dst
			   0x12, 0x01, 0x08,		// CMPAND[U] src RELOP dst, !TCy, TCx
			   0x12, 0x02, 0x00,		// CMPOR[U] src RELOP dst, TCy, TCx
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
