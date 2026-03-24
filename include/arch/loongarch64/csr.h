#ifndef CSR_H_
#define CSR_H_

#define CSR_CRMD        0x0
#define CSR_PRMD        0x1
#define CSR_EUEN        0x2
#define CSR_MISC        0x3
#define CSR_ECFG        0x4
#define CSR_ESTAT       0x5
#define CSR_ERA         0x6
#define CSR_BADV        0x7
#define CSR_BADI        0x8
#define CSR_EENTRY      0xC
#define STACK_SIZE      0xf8
#define ERA_OFFSET      0xf0

#define CSR_TLBIDX      0x10
#define CSR_TLBEHI      0x11
#define CSR_TLBELO0     0x12
#define CSR_TLBELO1     0x13
#define CSR_ASID        0x18

#define CSR_PGDL        0x19
#define CSR_PGDH        0x1A
#define CSR_PGD         0x1B
#define CSR_PWCL        0x1C
#define CSR_PWCH        0x1D
#define CSR_STLBPS      0x1E

#define CSR_CPUID       0x20

#define CSR_SAVE0       0x30
#define CSR_SAVE1       0x31
#define CSR_SAVE2       0x32
#define CSR_SAVE3       0x33

#define CSR_TID         0x40
#define CSR_TCFG        0x41
#define CSR_TVAL        0x42
#define CSR_CNTC        0x43
#define CSR_TICLR       0x44
#define CSR_LLBCTL      0x60
#define CSR_TLBRENTRY   0x88

#define CSR_MERA        0x90  
#define CSR_MECFG       0x91
#define CSR_MESTAT      0x92
#define CSR_MEBADV      0x93
#define CSR_MEBADI      0x94

#define CSR_DMW0        0x180
#define CSR_DMW1        0x181
#define CSR_DMW2        0x182
#define CSR_DMW3        0x183

#define DMW_MASK        0x9000000000000000UL
#define CSR_DMW0_PLV0   (1UL << 0)

#define CRMD_PLV        0x00000003
#define CRMD_PLV0       0x00000000
#define CRMD_PLV1       0x00000001  
#define CRMD_PLV2       0x00000002     
#define CRMD_PLV3       0x00000003     
#define CRMD_IE         0x00000004
#define CRMD_DA         0x00000008
#define CRMD_PG         0x00000010
#define CRMD_DATF       0x00000380
#define CRMD_DATM       0x00001c00
#define CRMD_DAC        0x00002000
#define CRMD_DBG        0x00004000
#define CRMD_SE         0x00008000 
#define CRMD_ICE        0x00010000   
#define CRMD_DCE        0x00020000

#define PRMD_PPLV       0x00000003
#define PRMD_PPLV0      0x00000000
#define PRMD_PPLV1      0x00000001
#define PRMD_PPLV2      0x00000002
#define PRMD_PPLV3      0x00000003
#define PRMD_PIE        0x00000004

#define EUEN_FPEN       0x00000000
#define EUEN_SXEN       0x00000010
#define EUEN_ASXEN      0x00000100

#define ECODE_INT       0x0
#define ECODE_PIL       0x1
#define ECODE_PIS       0x2
#define ECODE_PIF       0x3
#define ECODE_PME       0x4
#define ECODE_PPI       0x5
#define ECODE_ADEF      0x8
#define ECODE_ADEM      0x9
#define ECODE_ALE       0xA
#define ECODE_SYS       0xB
#define ECODE_BRK       0xC
#define ECODE_INE       0xD
#define ECODE_IPE       0xE
#define ECODE_FPD       0xF
#define ECODE_FPE       0x12

#define IRQ_SW0         0x0
#define IRQ_SW1         0x1
#define IRQ_HWI0        0x2
#define IRQ_HWI1        0x3
#define IRQ_HWI2        0x4
#define IRQ_HWI3        0x5
#define IRQ_HWI4        0x6
#define IRQ_HWI5        0x7
#define IRQ_HWI6        0x8
#define IRQ_HWI7        0x9
#define IRQ_TIMER       0xb
#define IRQ_IPI         0xc

#define ESTAT_TIMER     (1 << IRQ_TIMER)

#define TCFG_EN         0x0
#define TCFG_PERIOD     0x1
#define TCFG_INITVAL_SHIFT 2

#define PAGE_SHIFT      12U
#define PAGE_SIZE       (1U << PAGE_SHIFT)

#define PAGE_LEVEL_BITS 9

#define PTE_V           (1 << 0)
#define PTE_D           (1 << 1)
#define PTE_PLV         (3 << 2)
#define PTE_MAT         (3 << 4)
#define PTE_G           (1 << 6)
#define PTE_P           (1 << 7)
#define PTE_W           (1 << 8)
#define PTE_NR          (1 << 61)
#define PTE_NX          (1 << 62)

#define PTE_TABLE(PTE)  ((PTE) & PTE_V)

#define PWCL_PTBASE     12
#define PWCL_PTWIDTH    9
#define PWCL_DIR1BASE   21
#define PWCL_DIR1WIDTH  9
#define PWCL_DIR2BASE   30
#define PWCL_DIR2WIDTH  9
#define PWCL_PTEWIDTH   0  // 0 表示 64 比特，1 表示 128 比特，2 表示 256 比特，3 表示 512 比特。

#define PWCH_DIR3BASE   39
#define PWCH_DIR3WIDTH  9
#define PWCH_DIR4WIDTH  0

#define csr_read(csr)					                \
({								                        \
    unsigned long __rv;				                    \
    __asm__ volatile ("csrrd %0, %1"                    \
                      : "=r" (__rv)		                \
                      : "i"(csr));			            \
    __rv;							                    \
})

#define csr_write(csr, val)                             \
do {                                                    \
    unsigned long __wv = (unsigned long)(val);          \
    __asm__ volatile ("csrwr %0, %1"                    \
                      :                                 \
                      : "r"(__wv), "i"(csr)             \
                      : "memory");                      \
} while (0)

#define csr_xchg(csr, val, mask)                        \
({                                                      \
    unsigned long __tmp = (unsigned long)(val);         \
    __asm__ volatile ("csrxchg %0, %1, %2"              \
                      : "+r"(__tmp)                     \
                      : "r"(mask), "i"(csr));           \
    __tmp;                                              \
})

#define csr_read_set(csr, mask)                         \
({                                                      \
    unsigned long __mask = (unsigned long)(mask);       \
    unsigned long __val = csr_read(csr);                \
    csr_xchg(csr, __val | __mask, __mask);              \
    __val;                                              \
})

#define csr_set(csr, mask)                              \
do {                                                    \
    unsigned long __val = csr_read(csr);                \
    csr_xchg(csr, __val | (mask), (mask));              \
} while (0)

#define csr_read_clear(csr, mask)                       \
({                                                      \
    unsigned long __mask = (unsigned long)(mask);       \
    unsigned long __val = csr_read(csr);                \
    csr_xchg(csr, __val & ~__mask, __mask);             \
    __val;                                              \
})

#define csr_clear(csr, mask)                            \
do {                                                     \
    unsigned long __val = csr_read(csr);                \
    csr_xchg(csr, __val & ~(mask), (mask));             \
} while (0)

#define csr_swap(csr, val)                              \
({                                                      \
    unsigned long __v = (unsigned long)(val);           \
    unsigned long __old;                                \
    __old = csr_xchg(csr, __v, ~0UL);                   \
    __old;                                              \
})

#endif /* CSR_H_ */
