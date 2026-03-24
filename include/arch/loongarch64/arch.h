/**
 * @file
 * @brief LOONGARCH specific kernel interface header
 *
 * This header contains the LOONGARCH specific kernel interface.  It
 * is included by the kernel interface architecture-abstraction header
 * (include/zephyr/arch/cpu.h).
 */

#ifndef ZEPHYR_INCLUDE_ARCH_LOONGARCH_ARCH_H_
#define ZEPHYR_INCLUDE_ARCH_LOONGARCH_ARCH_H_

#include <arch/loongarch64/thread.h>
#include <arch/loongarch64/exception.h>
#include <arch/loongarch64/irq.h>
#include <arch/loongarch64/sys_io.h>
#if defined(CONFIG_USERSPACE)
#include <arch/loongarch64/syscall.h>
#endif /* CONFIG_USERSPACE */
#include <irq.h>
#include <sw_isr_table.h>
#include <arch/loongarch64/csr.h>
#include <arch/loongarch64/arch_inlines.h>
#include <arch/loongarch64/loongarch-privileged/asm_inline.h>

#define _arch_k_cycle_get_32() arch_k_cycle_get_32()

/* stacks, for LOONGARCH architecture stack should be 16byte-aligned */
#define ARCH_STACK_PTR_ALIGN  16
#define STACK_ALIGN 16

#define Z_LOONGARCH_STACK_PMP_ALIGN \
	MAX(CONFIG_PMP_GRANULARITY, ARCH_STACK_PTR_ALIGN)

#ifdef CONFIG_PMP_STACK_GUARD
/*
 * The StackGuard is an area at the bottom of the kernel-mode stack made to
 * fault when accessed. It is _not_ faulting when in exception mode as we rely
 * on that area to save the exception stack frame and to process said fault.
 * Therefore the guard area must be large enough to hold the esf, plus some
 * configurable stack wiggle room to execute the fault handling code off of,
 * as well as some guard size to cover possible sudden stack pointer
 * displacement before the fault.
 */
#ifdef CONFIG_PMP_POWER_OF_TWO_ALIGNMENT
#define Z_LOONGARCH_STACK_GUARD_SIZE \
	Z_POW2_CEIL(MAX(sizeof(struct arch_esf) + CONFIG_PMP_STACK_GUARD_MIN_SIZE, \
			Z_LOONGARCH_STACK_PMP_ALIGN))
#define ARCH_KERNEL_STACK_OBJ_ALIGN	Z_LOONGARCH_STACK_GUARD_SIZE
#else
#define Z_LOONGARCH_STACK_GUARD_SIZE \
	ROUND_UP(sizeof(struct arch_esf) + CONFIG_PMP_STACK_GUARD_MIN_SIZE, \
		 Z_LOONGARCH_STACK_PMP_ALIGN)
#define ARCH_KERNEL_STACK_OBJ_ALIGN	Z_LOONGARCH_STACK_PMP_ALIGN
#endif

/* Kernel-only stacks have the following layout if a stack guard is enabled:
 *
 * +------------+ <- thread.stack_obj
 * | Guard      | } Z_LOONGARCH_STACK_GUARD_SIZE
 * +------------+ <- thread.stack_info.start
 * | Kernel     |
 * | stack      |
 * |            |
 * +............|
 * | TLS        | } thread.stack_info.delta
 * +------------+ <- thread.stack_info.start + thread.stack_info.size
 */
#define ARCH_KERNEL_STACK_RESERVED	Z_LOONGARCH_STACK_GUARD_SIZE

#else /* !CONFIG_PMP_STACK_GUARD */
#define Z_LOONGARCH_STACK_GUARD_SIZE 0
#endif

#ifdef CONFIG_PMP_POWER_OF_TWO_ALIGNMENT
/* The privilege elevation stack is located in another area of memory
 * generated at build time by gen_kobject_list.py
 *
 * +------------+ <- thread.arch.priv_stack_start
 * | Guard      | } Z_LOONGARCH_STACK_GUARD_SIZE
 * +------------+
 * | Priv Stack | } CONFIG_PRIVILEGED_STACK_SIZE
 * +------------+ <- thread.arch.priv_stack_start +
 *                   CONFIG_PRIVILEGED_STACK_SIZE +
 *                   Z_LOONGARCH_STACK_GUARD_SIZE
 *
 * The main stack will be initially (or potentially only) used by kernel
 * mode so we need to make room for a possible stack guard area when enabled:
 *
 * +------------+ <- thread.stack_obj
 * | Guard      | } Z_LOONGARCH_STACK_GUARD_SIZE
 * +............| <- thread.stack_info.start
 * | Thread     |
 * | stack      |
 * |            |
 * +............|
 * | TLS        | } thread.stack_info.delta
 * +------------+ <- thread.stack_info.start + thread.stack_info.size
 *
 * When transitioning to user space, the guard area will be removed from
 * the main stack. Any thread running in user mode will have full access
 * to the region denoted by thread.stack_info. Make it PMP-NAPOT compatible.
 *
 * +------------+ <- thread.stack_obj = thread.stack_info.start
 * | Thread     |
 * | stack      |
 * |            |
 * +............|
 * | TLS        | } thread.stack_info.delta
 * +------------+ <- thread.stack_info.start + thread.stack_info.size
 */
#define ARCH_THREAD_STACK_RESERVED Z_LOONGARCH_STACK_GUARD_SIZE
#define ARCH_THREAD_STACK_SIZE_ADJUST(size) \
	Z_POW2_CEIL(MAX(MAX(size, CONFIG_PRIVILEGED_STACK_SIZE), \
			Z_LOONGARCH_STACK_PMP_ALIGN))
#define ARCH_THREAD_STACK_OBJ_ALIGN(size) \
		ARCH_THREAD_STACK_SIZE_ADJUST(size)

#else /* !CONFIG_PMP_POWER_OF_TWO_ALIGNMENT */

/* The stack object will contain the PMP guard, the privilege stack, and then
 * the usermode stack buffer in that order:
 *
 * +------------+ <- thread.stack_obj
 * | Guard      | } Z_LOONGARCH_STACK_GUARD_SIZE
 * +------------+
 * | Priv Stack | } CONFIG_PRIVILEGED_STACK_SIZE
 * +------------+ <- thread.stack_info.start
 * | Thread     |
 * | stack      |
 * |            |
 * +............|
 * | TLS        | } thread.stack_info.delta
 * +------------+ <- thread.stack_info.start + thread.stack_info.size
 */
#define ARCH_THREAD_STACK_RESERVED \
	ROUND_UP(Z_LOONGARCH_STACK_GUARD_SIZE + CONFIG_PRIVILEGED_STACK_SIZE, \
		 Z_LOONGARCH_STACK_PMP_ALIGN)
#define ARCH_THREAD_STACK_SIZE_ADJUST(size) \
	ROUND_UP(size, Z_LOONGARCH_STACK_PMP_ALIGN)
#define ARCH_THREAD_STACK_OBJ_ALIGN(size)	Z_LOONGARCH_STACK_PMP_ALIGN
#endif /* CONFIG_PMP_POWER_OF_TWO_ALIGNMENT */

#ifdef CONFIG_64BIT
#define LA_REGSIZE 8
#define LA_REGSHIFT 3
#else
#define LA_REGSIZE 4
#define LA_REGSHIFT 2
#endif

/* Keep RV_* names for compatibility with existing common code paths. */
#define RV_REGSIZE LA_REGSIZE
#define RV_REGSHIFT LA_REGSHIFT

/* Common CRMD bits used by irq lock/unlock logic. */
#define CRMD_IEN CRMD_IE

/* Default CRMD restore value for new threads. */
#define CRMD_DEF_RESTORE (CRMD_PLV0 | CRMD_IEN)

#ifndef _ASMLANGUAGE
#include <misc/util.h>

#define STACK_ROUND_UP(x) ROUND_UP(x, STACK_ALIGN)
#define STACK_ROUND_DOWN(x) ROUND_DOWN(x, STACK_ALIGN)

#ifdef __cplusplus
extern "C" {
#endif

#ifdef CONFIG_IRQ_VECTOR_TABLE_JUMP_BY_CODE
#define ARCH_IRQ_VECTOR_JUMP_CODE(v) "b " STRINGIFY(v)
#endif

extern void z_irq_spurious(const void *unused);
void _irq_spurious(void *unused);

/*
 * Use CSRXCHG on CRMD.IE to atomically lock global irq.
 */
static ALWAYS_INLINE unsigned int arch_irq_lock(void)
{
	return csr_xchg(CSR_CRMD, 0, CRMD_IEN);
}

/*
 * Restore CRMD.IE according to key returned by arch_irq_lock().
 */
static ALWAYS_INLINE void arch_irq_unlock(unsigned int key)
{
	(void)csr_xchg(CSR_CRMD, key & CRMD_IEN, CRMD_IEN);
}

static ALWAYS_INLINE bool arch_irq_unlocked(unsigned int key)
{
	return (key & CRMD_IEN) != 0U;
}

/* wrappers expected by common irq.h */
static ALWAYS_INLINE unsigned int _arch_irq_lock(void)
{
	return arch_irq_lock();
}

static ALWAYS_INLINE void _arch_irq_unlock(unsigned int key)
{
	arch_irq_unlock(key);
}

static ALWAYS_INLINE void arch_nop(void)
{
	__asm__ volatile("nop");
}

extern uint32_t sys_clock_cycle_get_32(void);

static inline uint32_t arch_k_cycle_get_32(void)
{
	return sys_clock_cycle_get_32();
}

extern uint64_t sys_clock_cycle_get_64(void);

static inline uint64_t arch_k_cycle_get_64(void)
{
	return sys_clock_cycle_get_64();
}

#ifdef __cplusplus
}
#endif

#endif /* _ASMLANGUAGE */

#endif /* ZEPHYR_INCLUDE_ARCH_LOONGARCH_ARCH_H_ */