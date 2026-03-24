/*
 * LoongArch64 Specific constants for ELF binaries
 *
 * Based on LoongArch ELF ABI specification
 */

#ifndef ZEPHYR_ARCH_LOONGARCH_ELF_H
#define ZEPHYR_ARCH_LOONGARCH_ELF_H

#include <stdint.h>
#include <zephyr/sys/util_macro.h>

#ifdef __cplusplus
extern "C" {
#endif

/*
 * Relocation types for LoongArch
 */

#define R_LARCH_NONE            0
#define R_LARCH_32              1
#define R_LARCH_64              2
#define R_LARCH_RELATIVE        3
#define R_LARCH_COPY            4
#define R_LARCH_JUMP_SLOT       5

/* TLS relocations */

#define R_LARCH_TLS_DTPMOD32    6
#define R_LARCH_TLS_DTPMOD64    7
#define R_LARCH_TLS_DTPREL32    8
#define R_LARCH_TLS_DTPREL64    9
#define R_LARCH_TLS_TPREL32     10
#define R_LARCH_TLS_TPREL64     11

/* PC-relative relocations */

#define R_LARCH_PCALA_HI20      12
#define R_LARCH_PCALA_LO12      13
#define R_LARCH_GOT_PC_HI20     14
#define R_LARCH_GOT_PC_LO12     15

/* Absolute address relocations */

#define R_LARCH_ABS_HI20        16
#define R_LARCH_ABS_LO12        17
#define R_LARCH_ABS64           18

/* Branch / jump relocations */

#define R_LARCH_B16             19
#define R_LARCH_B21             20
#define R_LARCH_B26             21

/* TLS */

#define R_LARCH_TLS_LE_HI20     22
#define R_LARCH_TLS_LE_LO12     23
#define R_LARCH_TLS_IE_PC_HI20  24
#define R_LARCH_TLS_IE_PC_LO12  25

/* Arithmetic relocations */

#define R_LARCH_ADD8            26
#define R_LARCH_ADD16           27
#define R_LARCH_ADD32           28
#define R_LARCH_ADD64           29

#define R_LARCH_SUB8            30
#define R_LARCH_SUB16           31
#define R_LARCH_SUB32           32
#define R_LARCH_SUB64           33

/*
 * Wordclass
 */

#if defined(CONFIG_64BIT)
typedef uint64_t r_loongarch_wordclass_t;
#else
typedef uint32_t r_loongarch_wordclass_t;
#endif


/*
 * Extract bit from immediate
 */

#define R_LARCH_IMM_GET_BIT(imm, bit) (((imm) & BIT(bit)) >> (bit))


/*
 * LoongArch branch encoding
 *
 * B-type instructions use:
 *
 * B16  -> 16 bit offset
 * B21  -> 21 bit offset
 * B26  -> 26 bit offset
 */


/* mask helpers */

#define R_LARCH_B16_MASK(imm)   (((imm) & 0xffff) << 10)

#define R_LARCH_B21_MASK(imm)   (((imm) & 0x1fffff) << 5)

#define R_LARCH_B26_MASK(imm)   (((imm) & 0x3ffffff) << 0)



/*
 * Clear immediate fields
 */

#define R_LARCH_CLEAR_B16(operand) \
    ((operand) & ~R_LARCH_B16_MASK((uint32_t)-1))

#define R_LARCH_CLEAR_B21(operand) \
    ((operand) & ~R_LARCH_B21_MASK((uint32_t)-1))

#define R_LARCH_CLEAR_B26(operand) \
    ((operand) & ~R_LARCH_B26_MASK((uint32_t)-1))


/*
 * Set immediate
 */

#define R_LARCH_SET_B16(operand, imm) \
    (R_LARCH_CLEAR_B16(operand) | R_LARCH_B16_MASK(imm))

#define R_LARCH_SET_B21(operand, imm) \
    (R_LARCH_CLEAR_B21(operand) | R_LARCH_B21_MASK(imm))

#define R_LARCH_SET_B26(operand, imm) \
    (R_LARCH_CLEAR_B26(operand) | R_LARCH_B26_MASK(imm))


/*
 * Load/store immediate (12 bit)
 */

#define R_LARCH_CLEAR_LO12(operand) \
    ((operand) & ~(0xfff << 10))

#define R_LARCH_SET_LO12(operand, imm) \
    (R_LARCH_CLEAR_LO12(operand) | ((imm & 0xfff) << 10))


/*
 * HI20 immediate
 */

#define R_LARCH_CLEAR_HI20(operand) \
    ((operand) & ~(0xfffff << 5))

#define R_LARCH_SET_HI20(operand, imm) \
    (R_LARCH_CLEAR_HI20(operand) | ((imm & 0xfffff) << 5))


#ifdef __cplusplus
}
#endif

#endif /* ZEPHYR_ARCH_LOONGARCH_ELF_H */