#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdarg.h>
#include <stdlib.h>
#include <unistd.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

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
		uint8_t		r;
		uint8_t		t;

		uint8_t		k3;
		uint8_t		k4;
		uint8_t		k5;
		uint8_t		k6;
		uint16_t	k8;
		uint16_t	k12;
		uint32_t	k16;

		uint8_t		l1;
		uint8_t		l3;
		uint8_t		l7;
		uint32_t	l16;

		uint16_t	K8;
		uint32_t	K16;

		uint8_t		L7;
		uint16_t	L8;
		uint32_t	L16;

		uint16_t	P8;
		uint32_t	P24;

		uint32_t	D16;

		uint8_t		SHFT;
		uint8_t		SHIFTW;

		uint8_t		ss;
		uint8_t		dd;

		uint8_t		cc;
		uint8_t		mm;
		uint8_t		vv;
		uint8_t		tt;

		uint8_t		XDDD;
		uint8_t		XSSS;

		uint8_t		FDDD;
		uint8_t		FSSS;

		uint8_t		SS[2];
		uint8_t		DD[2];

		uint8_t		CCCCCCC;
		uint16_t	AAAAAAAI;

		// aggregates

		uint8_t		Xmem_mmm;
		uint8_t		Xmem_reg;
		uint8_t		Ymem_mmm;
		uint8_t		Ymem_reg;
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

void dump(const void * p, int size)
{
	int i;
	char data[128];

	memset(data, ' ', sizeof(data));

	for (i = 0; i < size; i++)
		sprintf(data + i * 3, "%02X ", ((uint8_t *)p)[i]);

	printf("%-32s", data);
}

int run_f_list(insn_data_t * data, insn_item_t * insn)
{
	uint8_t temp;
	insn_flag_t * flag;

	if (!insn->f_list)
		return 0;

	for (flag = insn->f_list; !f_list_last(flag); flag++) {
		switch (flag->v) {
		case C55X_OPCODE_E:
			data->f.E = get_bits(data->opcode64, flag->f, 1);
			break;
		case C55X_OPCODE_R:
			data->f.R = get_bits(data->opcode64, flag->f, 1);
			break;
		case C55X_OPCODE_U:
			data->f.U = get_bits(data->opcode64, flag->f, 1);
			break;
		case C55X_OPCODE_u:
			data->f.u = get_bits(data->opcode64, flag->f, 1);
			break;
		case C55X_OPCODE_g:
			data->f.g = get_bits(data->opcode64, flag->f, 1);
			break;
		case C55X_OPCODE_r:
			data->f.r = get_bits(data->opcode64, flag->f, 1);
			break;
		case C55X_OPCODE_t:
			data->f.t = get_bits(data->opcode64, flag->f, 1);
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
		case C55X_OPCODE_k12:
			data->f.k12 = get_bits(data->opcode64, flag->f, 12);
			break;
		case C55X_OPCODE_k16:
			data->f.k16 = get_bits(data->opcode64, flag->f, 16);
			break;

		case C55X_OPCODE_l1:
			data->f.l1 = get_bits(data->opcode64, flag->f, 1);
			break;
		case C55X_OPCODE_l3:
			data->f.l3 = get_bits(data->opcode64, flag->f, 3);
			break;
		case C55X_OPCODE_l7:
			data->f.l7 = get_bits(data->opcode64, flag->f, 7);
			break;
		case C55X_OPCODE_l16:
			data->f.l16 = get_bits(data->opcode64, flag->f, 16);
			break;

		case C55X_OPCODE_K8:
			data->f.K8 = get_bits(data->opcode64, flag->f, 8);
			break;
		case C55X_OPCODE_K16:
			data->f.K16 = get_bits(data->opcode64, flag->f, 16);
			break;

		case C55X_OPCODE_L7:
			data->f.L7 = get_bits(data->opcode64, flag->f, 7);
			break;
		case C55X_OPCODE_L8:
			data->f.L8 = get_bits(data->opcode64, flag->f, 8);
			break;
		case C55X_OPCODE_L16:
			data->f.L16 = get_bits(data->opcode64, flag->f, 16);
			break;

		case C55X_OPCODE_P8:
			data->f.P8 = get_bits(data->opcode64, flag->f, 8);
			break;
		case C55X_OPCODE_P24:
			data->f.P24 = get_bits(data->opcode64, flag->f, 24);
			break;

		case C55X_OPCODE_D16:
			data->f.D16 = get_bits(data->opcode64, flag->f, 16);
			break;

		case C55X_OPCODE_SHFT:
			data->f.SHFT = get_bits(data->opcode64, flag->f, 4);
			break;
		case C55X_OPCODE_SHIFTW:
			data->f.SHIFTW = get_bits(data->opcode64, flag->f, 6);
			break;

		case C55X_OPCODE_CCCCCCC:
			data->f.CCCCCCC = get_bits(data->opcode64, flag->f, 7);
			break;
		case C55X_OPCODE_AAAAAAAI:
			data->f.AAAAAAAI = get_bits(data->opcode64, flag->f, 8);
			break;

		case C55X_OPCODE_cc:
			data->f.cc = get_bits(data->opcode64, flag->f, 2);
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
		case C55X_OPCODE_vv:
			data->f.vv = get_bits(data->opcode64, flag->f, 2);
			break;
		case C55X_OPCODE_tt:
			data->f.tt = get_bits(data->opcode64, flag->f, 2);
			break;

		case C55X_OPCODE_XSSS:
			data->f.XSSS = get_bits(data->opcode64, flag->f, 4);
			break;
		case C55X_OPCODE_XDDD:
			data->f.XDDD = get_bits(data->opcode64, flag->f, 4);
			break;
		case C55X_OPCODE_FSSS:
			data->f.FSSS = get_bits(data->opcode64, flag->f, 4);
			break;
		case C55X_OPCODE_FDDD:
			data->f.FDDD = get_bits(data->opcode64, flag->f, 4);
			break;

		case C55X_OPCODE_SS:
			temp = get_bits(data->opcode64, flag->f, 2);
			data->f.SS[1] = f_valid(data->f.SS[0]) ? temp : data->f.SS[1];
			data->f.SS[0] = f_valid(data->f.SS[0]) ? data->f.SS[0] : temp;
			break;
		case C55X_OPCODE_DD:
			temp = get_bits(data->opcode64, flag->f, 2);
			data->f.DD[1] = f_valid(data->f.DD[0]) ? temp : data->f.DD[1];
			data->f.DD[0] = f_valid(data->f.DD[0]) ? data->f.DD[0] : temp;
			break;

		case C55X_OPCODE_Y:
			temp = get_bits(data->opcode64, flag->f, 1) << 2;
			data->f.Ymem_reg = f_valid(data->f.Ymem_reg) ? data->f.Ymem_reg | temp : temp;
			break;
		case C55X_OPCODE_YY:
			temp = get_bits(data->opcode64, flag->f, 2) << 0;
			data->f.Ymem_reg = f_valid(data->f.Ymem_reg) ? data->f.Ymem_reg | temp : temp;
			break;
		case C55X_OPCODE_XXX:
			data->f.Xmem_reg = get_bits(data->opcode64, flag->f, 3);
			break;

		case C55X_OPCODE_MMM:
			temp = get_bits(data->opcode64, flag->f, 3);
			data->f.Ymem_mmm = f_valid(data->f.Xmem_mmm) ? temp : data->f.Ymem_mmm;
			data->f.Xmem_mmm = f_valid(data->f.Xmem_mmm) ? data->f.Xmem_mmm : temp;
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

const char * get_xreg_str(uint8_t key, char * str)
{
	static const char * table[16] = {
		"AC0", "AC1", "AC2", "AC3", "XSP", "XSSP", "XDP", "XCDP",
		"XAR0", "XAR1", "XAR2", "XAR3", "XAR4", "XAR5", "XAR6", "XAR7",
	};

	return table[ key & 15 ];
}

const char * get_freg_str(uint8_t key, char * str)
{
	static const char * table[16] = {
		"AC0", "AC1", "AC2", "AC3", "T0", "T1", "T2", "T3",
		"AR0", "AR1", "AR2", "AR3", "AR4", "AR5", "AR6", "AR7",
	};

	return table[ key & 15 ];
}

const char * get_swap_str(uint8_t key, char * str)
{
	strcpy(str, "reserved");

	switch (key) {
	case 0: return "SWAP AC0, AC2";
	case 1: return "SWAP AC1, AC3";
	case 4: return "SWAP T0, T2";
	case 5: return "SWAP T1, T3";
	case 8: return "SWAP AR0, AR2";
	case 9: return "SWAP AR1, AR3";
	case 12: return "SWAP AR4, T0";
	case 13: return "SWAP AR5, T1";
	case 14: return "SWAP AR6, T2";
	case 15: return "SWAP AR7, T3";
	case 16: return "SWAPP AC0, AC2";
	case 20: return "SWAPP T0, T2";
	case 24: return "SWAPP AR0, AR2";
	case 28: return "SWAPP AR4, T0";
	case 30: return "SWAPP AR6, T2";
	case 44: return "SWAP4 AR4, T0";
	case 56: return "SWAP AR0, AR1";
	}

	return str;
}

const char * get_relop_str(uint8_t key, char * str)
{
	static const char * table[] = {
		"==", "<", ">=", "!="
	};

	return table[ key & 3 ];
}

const char * get_cond_str(uint8_t key, char * str)
{
	strcpy(str, "reserved");

	/* 000 FSSS ... 101 FSSS */
	if ((key >> 4) >= 0 && (key >> 4) <= 5) {
		static const char * op[6] = { "==", "!=", "<", "<=", ">", ">=" };
		sprintf(str, "%s %s 0", get_freg_str(key, NULL), op[(key >> 4) & 7]);
		return str;
	}

	/* 110 00SS */
	if ((key >> 2) == 0x18) {
		sprintf(str, "overflow(AC%d)", key & 3);
		return str;
	}

	/* 111 00SS */
	if ((key >> 2) == 0x1C) {
		sprintf(str, "!overflow(AC%d)", key & 3);
		return str;
	}

	switch (key) {
	case 0x64: return "TC1";
	case 0x65: return "TC2";
	case 0x66: return "CARRY";
	case 0x74: return "!TC1";
	case 0x75: return "!TC2";
	case 0x76: return "!CARRY";
		/* "&" operation */
	case 0x68: return "TC1 & TC2";
	case 0x69: return "TC1 & !TC2";
	case 0x6A: return "!TC1 & TC2";
	case 0x6B: return "!TC1 & !TC2";
		/* "|" operation */
	case 0x78: return "TC1 | TC2";
	case 0x79: return "TC1 | !TC2";
	case 0x7A: return "!TC1 | TC2";
	case 0x7B: return "!TC1 | !TC2";
		/* "^" operation */
	case 0x7C: return "TC1 ^ TC2";
	case 0x7D: return "TC1 ^ !TC2";
	case 0x7E: return "!TC1 ^ TC2";
	case 0x7F: return "!TC1 ^ !TC2";
	}

	return str;
}

const char * get_v_str(uint8_t key, char * str)
{
	static const char * table[2] = {
		"CARRY", "TC2",
	};

	return table[ key & 1 ];
}

const char * get_t_str(uint8_t key, char * str)
{
	static const char * table[2] = {
		"TC1", "TC2",
	};

	return table[ key & 1 ];
}

const char * get_cmem_str(uint8_t key, char * str)
{
	static const char * table[4] = {
		"*CDP", "*CDP+", "*CDP-", "*(CDP+T0)",
	};

	return table[ key & 3 ];
}

const char * get_mmm_str(uint8_t key, char * str)
{
	switch (key & 7) {
	case 0: return "*ARn";
	case 1: return "*ARn+";
	case 2: return "*ARn−";
		// TODO:
		//	C54CM:0 => *(ARn + T0)
		//	C54CM:1 => *(ARn + AR0)
	case 3: return "*(ARn + T0)";
	case 4: return "*(ARn + T1)";
		// TODO:
		//	C54CM:0 => *(ARn - T0)
		//	C54CM:1 => *(ARn - AR0)
	case 5: return "*(ARn - T0)";
	case 6: return "*(ARn - T1)";
		// TODO:
		//	C54CM:0 => *ARn(T0)
		//	C54CM:1 => *ARn(AR0)
	case 7: return "*ARn(T0)";
	};
}

/*
 * syntax decoders
 */

#define field_value(n)		data->f.n
#define field_valid(n)		f_valid(field_value(n))

void decode_bits(insn_data_t * data, char * str)
{
	// rounding
	if (field_valid(R))
		substitute(str, "[R]", "%s", field_value(R) ? "R" : "");

	// UNSigned keyword
	if (field_valid(u))
		substitute(str, "[U]", "%s", field_value(u) ? "U" : "");

	// 40 keyword
	if (field_valid(g))
		substitute(str, "[40]", "%s", field_value(g) ? "40" : "");

	// T3 update
	if (field_valid(U))
		substitute(str, "[T3 = ]", "%s", field_value(U) ? "T3=" : "");
}

void decode_constants(insn_data_t * data, char * str)
{
	// signed constant

	if (field_valid(K8))
		substitute(str, "K8", "#%02Xh", field_value(K8));
	if (field_valid(K16))
		substitute(str, "K16", "#%04Xh", field_value(K16));

	// unsigned constant

	if (field_valid(k4))
		substitute(str, "k4", "#%02Xh", field_value(k4));
	if (field_valid(k5))
		substitute(str, "k5", "#%02Xh", field_value(k5));
	if (field_valid(k8))
		substitute(str, "k8", "#%02Xh", field_value(k8));

	if (field_valid(k12))
		substitute(str, "k12", "#%03Xh", field_value(k12));
	if (field_valid(k16))
		substitute(str, "k16", "#%04Xh", field_value(k16));

	if (field_valid(k4) && field_valid(k3))
		substitute(str, "k7", "#%02Xh", (field_value(k3) << 4) | field_value(k4));
	if (field_valid(k4) && field_valid(k5))
		substitute(str, "k9", "#%03Xh", (field_value(k5) << 4) | field_value(k4));

	// data address label

	if (field_valid(D16))
		substitute(str, "D16", "%04Xh", field_value(D16));

	// immediate shift value

	if (field_valid(SHFT))
		substitute(str, "SHFT", "%02Xh", field_value(SHFT));
	if (field_valid(SHIFTW))
		substitute(str, "SHIFTW", "%02Xh", field_value(SHIFTW));
}

void decode_addresses(insn_data_t * data, char * str)
{
	// program address label

	if (field_valid(L7))
		substitute(str, "L7", "%02Xh", field_value(L7));
	if (field_valid(L8))
		substitute(str, "L8", "%02Xh", field_value(L8));
	if (field_valid(L16))
		substitute(str, "L16", "%04Xh", field_value(L16));

	// program address label

	if (field_valid(l1) && field_valid(l3))
		substitute(str, "l4", "%02Xh", (field_value(l3) << 1) | field_value(l1));

	// program memory address

	if (field_valid(l7))
		substitute(str, "pmad", "%02Xh", field_value(l7));
	if (field_valid(l16))
		substitute(str, "pmad", "%04Xh", field_value(l16));

	// program or data address label

	if (field_valid(P8))
		substitute(str, "P8", "%02Xh", field_value(P8));
	if (field_valid(P24))
		substitute(str, "P24", "%06Xh", field_value(P24));
}

void decode_swap(insn_data_t * data, char * str)
{
	char tmp[64];

	if (field_valid(k6))
		substitute(str, "SWAP ( )", get_swap_str(field_value(k6), tmp));
}

void decode_relop(insn_data_t * data, char * str)
{
	if (field_valid(cc))
		substitute(str, "RELOP", get_relop_str(field_value(cc), str));
}

void decode_cond(insn_data_t * data, char * str)
{
	char tmp[64];

	if (field_valid(CCCCCCC))
		substitute(str, "cond", "%s", get_cond_str(field_valid(CCCCCCC), tmp));
}

void decode_registers(insn_data_t * data, char * str)
{
	uint8_t code = 0;

	// transition register

	if (field_valid(r))
		substitute(str, "TRNx", "TRN%d", field_value(r));

	// source and destination temporary registers

	if (field_valid(ss))
		substitute(str, "Tx", "T%d", field_value(ss));

	if (field_valid(dd))
		substitute(str, "Tx", "T%d", field_value(dd));

	// shifted in/out bit values

	if (field_valid(vv)) {
		substitute(str, "BitIn", "%s", get_v_str(field_value(vv) >> 1, NULL));
		substitute(str, "BitOut", "%s", get_v_str(field_value(vv) >> 0, NULL));
	}

	// source and destination of CRC instruction

	if (field_valid(t))
		substitute(str, "TCx", "%s", get_t_str(field_value(t), NULL));

	if (field_valid(tt)) {
		substitute(str, "TCx", "%s", get_t_str(field_value(tt) >> 1, NULL));
		substitute(str, "TCy", "%s", get_t_str(field_value(tt) >> 0, NULL));
	}

	// source or destination accumulator or extended register

	if (field_valid(XSSS)) {
		substitute(str, "xsrc", "%s", get_xreg_str(field_value(XSSS), NULL));
		substitute(str, "XAsrc", "%s", get_xreg_str(field_value(XSSS), NULL));
	}

	if (field_valid(XDDD)) {
		substitute(str, "xdst", "%s", get_xreg_str(field_value(XDDD), NULL));
		substitute(str, "XAdst", "%s", get_xreg_str(field_value(XDDD), NULL));
	}

	// source or destination accumulator, auxiliary or temporary register

	if (field_valid(FSSS) && field_valid(FDDD)) {
		if (field_value(FSSS) == field_value(FDDD))
			substitute(str, "[src,] dst", "dst");
		else
			substitute(str, "[src,] dst", "src, dst");
	}

	if (field_valid(FSSS)) {
		substitute(str, "src1", "%s", get_freg_str(field_value(FSSS), NULL));
		substitute(str, "src", "%s", get_freg_str(field_value(FSSS), NULL));
		substitute(str, "TAy", "%s", get_freg_str(field_value(FSSS), NULL));
	}

	if (field_valid(FDDD)) {
		substitute(str, "dst1", "%s", get_freg_str(field_value(FDDD), NULL));
		substitute(str, "dst", "%s", get_freg_str(field_value(FDDD), NULL));
		substitute(str, "TAx", "%s", get_freg_str(field_value(FDDD), NULL));
	}

	// source and destination accumulator registers

	code |= field_valid(SS[0]) ? 0x01 : 0x00;
	code |= field_valid(SS[1]) ? 0x02 : 0x00;
	code |= field_valid(DD[0]) ? 0x10 : 0x00;
	code |= field_valid(DD[1]) ? 0x20 : 0x00;

	switch (code) {
	case 0x01:	// SS
		substitute(str, "ACx", "AC%d", field_value(SS[0]));
		break;
	case 0x03:	// SSSS
		substitute(str, "ACx", "AC%d", field_value(SS[0]));
		substitute(str, "ACy", "AC%d", field_value(SS[1]));
		break;
	case 0x11:	// SS   DD
		if (field_value(SS[0]) == field_value(DD[0])) {
			substitute(str, "[ACx,] ACy", "AC%d", field_value(SS[0]));
		} else {
			substitute(str, "[ACx,] ACy", "AC%d, AC%d", field_value(SS[0]), field_value(DD[0]));
		}
		break;
	case 0x33:	// SSSS DDDD
		substitute(str, "ACx", "AC%d", field_value(SS[0]));
		substitute(str, "ACy", "AC%d", field_value(SS[1]));
		substitute(str, "ACz", "AC%d", field_value(DD[0]));
		substitute(str, "ACw", "AC%d", field_value(DD[1]));
		break;
	case 0x10:	//      DD
		substitute(str, "ACx", "AC%d", field_value(DD[0]));
		break;
	case 0x30:	//      DDDD
		substitute(str, "ACx", "AC%d", field_value(DD[0]));
		substitute(str, "ACy", "AC%d", field_value(DD[1]));
		break;
	}
}

void decode_addressing_modes(insn_data_t * data, char * str)
{
	if (field_valid(mm))
		substitute(str, "Cmem", "%s", get_cmem_str(field_value(mm), NULL));

	// Xmem and Ymem

	if (field_valid(Xmem_reg) && field_valid(Xmem_mmm)) {
		substitute(str, "Xmem", "%s", get_mmm_str(field_value(Xmem_mmm), NULL));
		substitute(str, "ARn", "AR%d", field_value(Xmem_reg));
	}

	if (field_valid(Ymem_reg) && field_valid(Ymem_mmm)) {
		substitute(str, "Ymem", "%s", get_mmm_str(field_value(Ymem_mmm), NULL));
		substitute(str, "ARn", "AR%d", field_value(Ymem_reg));
	}
}

void decode_insn_syntax(insn_data_t * data, insn_item_t * insn)
{
	char syntax[1024];

	snprintf(syntax, sizeof(syntax), \
		 field_valid(E) ? "|| %s" : "%s", insn->syntax);

	decode_bits(data, syntax);

	decode_constants(data, syntax);
	decode_addresses(data, syntax);

	decode_swap(data, syntax);
	decode_relop(data, syntax);
	decode_cond(data, syntax);

	decode_registers(data, syntax);
	decode_addressing_modes(data, syntax);

	printf("\t%s\n", syntax);
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

	if (insn && insn->syntax) {
		dump(data->stream, data->head->size);
		decode_insn_syntax(data, insn);
	} else {
		printf("%-32s MY_BYTE %02Xh\n", "", data->stream[0]);
	}

	return (insn && insn->syntax) ? data->head->size : -1;
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
//		dump(stream, 1);
		printf("\tMY_BYTE %02Xh\n", stream[0]);
		return -1;
	}

//	dump(stream, data.head->size);

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
#if 1
	int length;
	uint8_t data[] = { 0x20,			// NOP
			   0x21,			// NOP E
//			   0xFD,			// invalid
			   0xCB, 0x88,			//
			   0x5F, 0x00,			// SWAP (...)
			   0x60, 0x07,			// BCC l4, cond
			   0x16, 0x07, 0xF0,		// MOV #7Fh, DPH
			   0x00, 0x00, 0xFF,		// RPTCC #FFh, cond
			   0x12, 0x00, 0x00,		// CMP[U] src RELOP dst, TCx
			   0x12, 0x03, 0x01,		// ROL TC2, src, CARRY, dst
			   0x12, 0x03, 0x09,		// ROR CARRY, src, TC2, dst
			   0x12, 0x01, 0x08,		// CMPAND[U] src RELOP dst, !TCy, TCx
			   0x12, 0x02, 0x00,		// CMPOR[U] src RELOP dst, TCy, TCx
			   0x90, 0x5A,			// MOV xsrc, xdst
			   0x56, 0xFF,			// MAC[R] ...
			   0xFA, 0x00, 0x00, 0x04,	// MOV [rnd ...
			   0xFD, 0x00, 0x00, 0x00,	// MPY[R] ... :: MPY[R] ...
			   0xFD, 0xff, 0x04, 0x00,	// MPY[R] ... :: MAC[R] ...
			   0xFF }, * p = data;

	initialize();

	while ((length = decode(p)) > 0) {
		p += length;
	}
#else
	uint8_t * data;
	int fd, count, address = 0;

	initialize();

	data = (uint8_t *)malloc(1024 * 64);
	if (!data)
		return -1;

	fd = open("test/c55x_random.bin", 0);
	if (fd == -1)
		return -2;

	count = read(fd, data, 1024 * 64);
	while (address < count) {
		int len;

		printf("%04X: ", address);
		len = decode(data + address);
		if (len < 0) {
			address += 1;
		} else {
			address += len;
		}
	}
#endif
	return 0;
}
