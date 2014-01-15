#ifndef __C55XDE_H__
#define __C55XDE_H__

/*
 * Instruction Set Opcode Symbols and Abbrevations
 */

#define C55X_OPCODE_R			1	/* Rounding				*/
#define C55X_OPCODE_AAAAAAAI		2	/* Smem addressing mode			*/
#define C55X_OPCODE_cc			3	/* Relational operators			*/
#define C55X_OPCODE_CCCCCCC		4	/* Conditional field			*/
#define C55X_OPCODE_dd			5	/* Destination temporary register	*/
#define C55X_OPCODE_DD			6	/* Destination accumulator register	*/
#define C55X_OPCODE_D16			7	/* Data address label			*/
#define C55X_OPCODE_E			8	/* Parallel Enable bit			*/
#define C55X_OPCODE_FDDD		9	/* Destination or Source		*/
#define C55X_OPCODE_FSSS		10	/* Destination or Source		*/
#define C55X_OPCODE_g			11	/* 40 keyword				*/

#define C55X_OPCODE_kkkkkk		12	/* Swap code				*/

#define C55X_OPCODE_k4			13	/* [u] constant of 4 bits		*/
#define C55X_OPCODE_k5			14	/*              .. 5 bits		*/
#define C55X_OPCODE_k7			15	/*              .. 7 bits		*/
#define C55X_OPCODE_k8			16	/*              .. 8 bits		*/
#define C55X_OPCODE_k9			17	/*              .. 9 bits		*/
#define C55X_OPCODE_k12			18	/*              .. 12 bits		*/
#define C55X_OPCODE_k16			19	/*              .. 16 bits		*/

#define C55X_OPCODE_K8			20	/* [S] constant of 8 bits		*/
#define C55X_OPCODE_K16			21	/*              .. 16 bits		*/

#define C55X_OPCODE_l4			22	/* [u] program address label in 4 bits	*/
#define C55X_OPCODE_l7			23	/*                           .. 7 bits	*/
#define C55X_OPCODE_l16			24	/*                           .. 16 bits	*/

#define C55X_OPCODE_L7			25	/* [S] program address label in 7 bits	*/
#define C55X_OPCODE_L8			26	/*                           .. 8 bits	*/
#define C55X_OPCODE_L16			27	/*                           .. 16 bits	*/

#define C55X_OPCODE_mm			28	/* Coefficient addressing mode (Cmem)	*/
#define C55X_OPCODE_MMM			29	/* Modifier option for Xmem or Ymem	*/
#define C55X_OPCODE_n			30	/* Reserverd bit			*/

#define C55X_OPCODE_P8			31	/* Program or data address in 8 bits	*/
#define C55X_OPCODE_P16			32	/*                         .. 16 bits	*/
#define C55X_OPCODE_P24			33	/*                         .. 24 bits	*/

#define C55X_OPCODE_r			34	/* TRNx					*/

#define C55X_OPCODE_SHFT		35	/* 4-bit immediate shift value		*/
#define C55X_OPCODE_SHIFTW		36	/* 6-bit immediate shift value		*/

#define C55X_OPCODE_ss			37	/* Source temporary register		*/
#define C55X_OPCODE_SS			38	/* Source accumulator register		*/

#define C55X_OPCODE_tt			39	/* ?..					*/

#define C55X_OPCODE_u			40	/* U or uns keyword			*/

#define C55X_OPCODE_U			41	/* T3 update selector			*/

#define C55X_OPCODE_vv			42	/* ?..					*/

#define C55X_OPCODE_x			43	/* Reserved bit				*/

#define C55X_OPCODE_XDDD		44	/* Destination or ..			*/
#define C55X_OPCODE_XSSS		45	/*          .. or Source		*/

#define C55X_OPCODE_XXX			46	/* Auxillary register designation Xmem	*/
#define C55X_OPCODE_YYY			47	/* Auxillary register designation Ymem	*/

#endif /* __C55XDE_H__ */
