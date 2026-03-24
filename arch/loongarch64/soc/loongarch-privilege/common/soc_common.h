/*
 * SPDX-License-Identifier: Apache-2.0
 */

/**
 * @file Configuration macros for LoongArch privilege-mode SoCs
 */

#ifndef __SOC_COMMON_H_
#define __SOC_COMMON_H_

/* IRQ numbers (ESTAT.IS) */
#define LA_IRQ_SW0          0   /* Software interrupt 0 */
#define LA_IRQ_SW1          1   /* Software interrupt 1 */
#define LA_IRQ_HWI0         2
#define LA_IRQ_HWI1         3
#define LA_IRQ_HWI2         4
#define LA_IRQ_HWI3         5
#define LA_IRQ_HWI4         6
#define LA_IRQ_HWI5         7
#define LA_IRQ_HWI6         8
#define LA_IRQ_HWI7         9
#define LA_IRQ_TIMER        11
#define LA_IRQ_IPI          12

#define LA_MAX_GENERIC_IRQ  12

/* Exception codes (ESTAT.ECODE) */
#define LA_ECODE_INT        0x0
#define LA_ECODE_ADEF       0x8
#define LA_ECODE_ADEM       0x9
#define LA_ECODE_ALE        0xA
#define LA_ECODE_SYS        0xB
#define LA_ECODE_BRK        0xC
#define LA_ECODE_INE        0xD
#define LA_ECODE_IPE        0xE
#define LA_ECODE_FPD        0xF
#define LA_ECODE_FPE        0x12

/* CSR names for save/restore */
#define SOC_CRMD_REG        CSR_CRMD
#define SOC_PRMD_REG        CSR_PRMD
#define SOC_ERA_REG         CSR_ERA
#define SOC_ESTAT_REG       CSR_ESTAT
#define SOC_ECFG_REG        CSR_ECFG

/* CRMD bits */
#define SOC_CRMD_IE         CRMD_IE
#define SOC_CRMD_PLV_MASK   CRMD_PLV

/* Default CRMD restore value for first schedule: enable IE, stay at PLV0 */
#define SOC_CRMD_DEF_RESTORE    (CRMD_IE | CRMD_PLV0)

/* ESTAT bits */
#define SOC_ESTAT_IRQ_MASK      LOONGARCH_ESTAT_IS_MASK

/* Exception return instruction */
#define SOC_ERTN                ertn

#ifndef _ASMLANGUAGE
#include <arch/loongarch64/csr.h>

#if defined(CONFIG_LOONGARCH_SOC_INTERRUPT_INIT)
void soc_interrupt_init(void);
#endif

#endif /* !_ASMLANGUAGE */

#endif /* __SOC_COMMON_H_ */
