#ifndef __C55XDE_H__
#define __C55XDE_H__

/*
 * Instruction Set Opcode Symbols and Abbrevations
 */

typedef enum {

    /* 1bl */

    C55X_OPCODE_E,
    C55X_OPCODE_R,
    C55X_OPCODE_U,
    C55X_OPCODE_Y,

    C55X_OPCODE_u,
    C55X_OPCODE_g,
    C55X_OPCODE_r,
    C55X_OPCODE_n,
    C55X_OPCODE_x,
    C55X_OPCODE_t,

    /* 2bl */

    C55X_OPCODE_mm,
    C55X_OPCODE_cc,
    C55X_OPCODE_ss,
    C55X_OPCODE_dd,
    C55X_OPCODE_tt,
    C55X_OPCODE_vv,
    C55X_OPCODE_SS,
    C55X_OPCODE_DD,
    C55X_OPCODE_YY,

    /* 3bl */

    C55X_OPCODE_k3,
    C55X_OPCODE_MMM,
    C55X_OPCODE_XXX,
    C55X_OPCODE_YYY,

    /* 4bl */

    C55X_OPCODE_k4,
    C55X_OPCODE_l4,
    C55X_OPCODE_FDDD,
    C55X_OPCODE_FSSS,
    C55X_OPCODE_SHFT,
    C55X_OPCODE_XDDD,
    C55X_OPCODE_XSSS,
    C55X_OPCODE_XACS,
    C55X_OPCODE_XACD,

    /* 5bl */

    C55X_OPCODE_k5,

    /* 6bl */

    C55X_OPCODE_k6,
    C55X_OPCODE_SHIFTW,

    /* 7bl */

    C55X_OPCODE_k7,
    C55X_OPCODE_l7,
    C55X_OPCODE_C7,
    C55X_OPCODE_L7,

    /* 8bl */

    C55X_OPCODE_k8,
    C55X_OPCODE_K8,
    C55X_OPCODE_L8,
    C55X_OPCODE_P8,
    C55X_OPCODE_AAAAAAAI,

    /* 9bl */

    C55X_OPCODE_k9,

    /* 12bl */

    C55X_OPCODE_k12,

    /* 16bl */

    C55X_OPCODE_k16,
    C55X_OPCODE_l16,
    C55X_OPCODE_K16,
    C55X_OPCODE_L16,
    C55X_OPCODE_D16,
    C55X_OPCODE_P16,

    /* 24bl */

    C55X_OPCODE_P24,

    __C55_OPCODE_LAST
} insn_flag_e;

#endif /* __C55XDE_H__ */
