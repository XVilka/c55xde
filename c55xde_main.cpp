#include <stdio.h>
#include <stdint.h>

#include "c55xde.h"

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

int main(int argc, const char * argv[])
{
	return 0;
}
