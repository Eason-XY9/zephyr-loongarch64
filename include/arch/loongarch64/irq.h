/**
 * @file
 * @brief LoongArch public interrupt handling
 *
 * LoongArch-specific kernel interrupt handling interface.
 */

#ifndef ZEPHYR_INCLUDE_ARCH_LOONGARCH_IRQ_H_
#define ZEPHYR_INCLUDE_ARCH_LOONGARCH_IRQ_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <misc/util.h>

#ifndef _ASMLANGUAGE

#include <irq.h>
#include <sw_isr_table.h>
#include <stdbool.h>
#include <arch/loongarch64/csr.h>

#endif /* !_ASMLANGUAGE */


/* ========================================================= */
/* Exception Codes (来自 ESTAT.ECODE) */
/* ========================================================= */

#define LOONGARCH_EXC_INT      0x0
#define LOONGARCH_EXC_PIL      0x1
#define LOONGARCH_EXC_PIS      0x2
#define LOONGARCH_EXC_PIF      0x3
#define LOONGARCH_EXC_PME      0x4
#define LOONGARCH_EXC_PPI      0x5
#define LOONGARCH_EXC_ADEF     0x8
#define LOONGARCH_EXC_ADEM     0x9
#define LOONGARCH_EXC_ALE      0xA
#define LOONGARCH_EXC_SYS      0xB
#define LOONGARCH_EXC_BRK      0xC
#define LOONGARCH_EXC_INE      0xD
#define LOONGARCH_EXC_IPE      0xE
#define LOONGARCH_EXC_FPD      0xF
#define LOONGARCH_EXC_FPE      0x12


/* ========================================================= */
/* IRQ Source (ESTAT.IS bits) */
/* ========================================================= */

#define LOONGARCH_IRQ_SW0      0
#define LOONGARCH_IRQ_SW1      1
#define LOONGARCH_IRQ_HWI0     2
#define LOONGARCH_IRQ_HWI1     3
#define LOONGARCH_IRQ_HWI2     4
#define LOONGARCH_IRQ_HWI3     5
#define LOONGARCH_IRQ_HWI4     6
#define LOONGARCH_IRQ_HWI5     7
#define LOONGARCH_IRQ_HWI6     8
#define LOONGARCH_IRQ_HWI7     9
#define LOONGARCH_IRQ_TIMER    11
#define LOONGARCH_IRQ_IPI      12


/* ========================================================= */
/* ESTAT bit helpers */
/* ========================================================= */

#define LOONGARCH_ESTAT_IS_MASK    0x1fff
#define LOONGARCH_ESTAT_ECODE_SHIFT 16
#define LOONGARCH_ESTAT_ECODE_MASK (0x3f << 16)


#ifndef _ASMLANGUAGE


/* ========================================================= */
/* IRQ control interface */
/* ========================================================= */

extern void arch_irq_enable(unsigned int irq);
extern void arch_irq_disable(unsigned int irq);
extern int arch_irq_is_enabled(unsigned int irq);


/* ========================================================= */
/* Priority interface (暂时空实现) */
/* ========================================================= */

#define z_loongarch_irq_priority_set(i, p, f) /* nothing */


/* ========================================================= */
/* IRQ connect macros */
/* ========================================================= */

#define ARCH_IRQ_CONNECT(irq_p, priority_p, isr_p, isr_param_p, flags_p) \
{ \
    Z_ISR_DECLARE(irq_p, 0, isr_p, isr_param_p); \
    z_loongarch_irq_priority_set(irq_p, priority_p, flags_p); \
}

#define ARCH_IRQ_DIRECT_CONNECT(irq_p, priority_p, isr_p, flags_p) \
{ \
    Z_ISR_DECLARE_DIRECT(irq_p, ISR_FLAG_DIRECT, isr_p); \
    z_loongarch_irq_priority_set(irq_p, priority_p, flags_p); \
}


/* ========================================================= */
/* ISR tracing */
/* ========================================================= */

#ifdef CONFIG_TRACING_ISR
extern void sys_trace_isr_enter(void);
extern void sys_trace_isr_exit(void);
#endif


/* ========================================================= */
/* ISR entry */
/* ========================================================= */

static inline void arch_isr_direct_header(void)
{
#ifdef CONFIG_TRACING_ISR
    sys_trace_isr_enter();
#endif
}


/* ========================================================= */
/* SoC IRQ handler */
/* ========================================================= */

extern void __soc_handle_irq(unsigned long irq);


/* ========================================================= */
/* ISR exit */
/* ========================================================= */

static inline void arch_isr_direct_footer(int swap)
{
    ARG_UNUSED(swap);

    unsigned long estat;

    estat = csr_read(CSR_ESTAT);

    unsigned long irq = estat & LOONGARCH_ESTAT_IS_MASK;

    __soc_handle_irq(irq);

#ifdef CONFIG_TRACING_ISR
    sys_trace_isr_exit();
#endif
}


/* ========================================================= */
/* Direct ISR declaration */
/* ========================================================= */

#define ARCH_ISR_DIRECT_DECLARE(name) \
    static inline int name##_body(void); \
    void name(void) \
    { \
        ISR_DIRECT_HEADER(); \
        name##_body(); \
        ISR_DIRECT_FOOTER(0); \
    } \
    static inline int name##_body(void)


#endif /* _ASMLANGUAGE */


#ifdef __cplusplus
}
#endif

#endif /* ZEPHYR_INCLUDE_ARCH_LOONGARCH_IRQ_H_ */