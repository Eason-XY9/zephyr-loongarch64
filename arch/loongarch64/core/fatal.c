/*
 * Copyright (c) 2024
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <kernel.h>
#include <kernel_structs.h>
#include <inttypes.h>
#include <toolchain.h>
#include <misc/util.h>
#include <misc/printk.h>
#include <arch/loongarch64/exception.h>
#include <arch/loongarch64/irq.h>
#include <arch/loongarch64/csr.h>

#ifdef CONFIG_LOONGARCH_PMP
#include <arch/loongarch64/pmp.h>
#endif

#define EXCEPTION_DUMP(...) printk(__VA_ARGS__)

#ifndef CONFIG_64BIT
 #define PR_REG "%08" PRIxPTR
 #define NO_REG "        "
#else
 #define PR_REG "%016" PRIxPTR
 #define NO_REG "                "
#endif

/* Stack trace function */
void z_loongarch_unwind_stack(const struct arch_esf *esf, const _callee_saved_t *csf);

uintptr_t z_loongarch_get_sp_before_exc(const struct arch_esf *esf)
{
	/*
	 * Kernel stack pointer prior this exception i.e. before
	 * storing the exception stack frame.
	 */
	uintptr_t sp = (uintptr_t)esf + sizeof(struct arch_esf);

#ifdef CONFIG_USERSPACE
	if ((esf->crmd & 0x3) == 3) {
		/*
		 * Exception happened in user space:
		 * consider the saved user stack instead.
		 */
		sp = esf->sp;
	}
#endif

	return sp;
}

const char *z_loongarch_estat_ecode_str(unsigned long ecode)
{
	static const char *const ecode_str[] = {
		[0] = "Interrupt",
		[1] = "Page invalid exception for load",
		[2] = "Page invalid exception for store",
		[3] = "Page invalid exception for fetch",
		[4] = "Page modification exception",
		[5] = "Page privilege exception",
		[6] = "unknown",
		[7] = "unknown",
		[8] = "Address error exception for fetch",
		[9] = "Address error exception for memory access",
		[10] = "Address alignment fault exception",
		[11] = "System call exception",
		[12] = "Breakpoint exception",
		[13] = "Instruction non-existent exception",
		[14] = "Instruction privilege exception",
		[15] = "Floating-point unit disable exception",
		[16] = "unknown",
		[17] = "unknown",
		[18] = "Floating-point exception",
	};

	return ecode_str[min(ecode, ARRAY_SIZE(ecode_str) - 1)];
}

void z_loongarch_fatal_error(unsigned int reason,
				       const struct arch_esf *esf)
{
	_callee_saved_t *csf = NULL;
	ARG_UNUSED(csf);
	unsigned long estat;

	__asm__ volatile("csrrd %0, %1" : "=r" (estat) : "i" (CSR_ESTAT));

	unsigned long ecode = (estat & LOONGARCH_ESTAT_ECODE_MASK) >> LOONGARCH_ESTAT_ECODE_SHIFT;
	EXCEPTION_DUMP("");
	EXCEPTION_DUMP(" estat: %ld, ecode: %ld (%s)", estat, ecode, z_loongarch_estat_ecode_str(ecode));

	unsigned long badv;
	__asm__ volatile("csrrd %0, %1" : "=r" (badv) : "i" (CSR_BADV));
	EXCEPTION_DUMP("  badv: %lx", badv);

#ifdef CONFIG_EXCEPTION_DEBUG
	if (esf != NULL) {
		EXCEPTION_DUMP("      ra: " PR_REG "      fp: " PR_REG, esf->ra, esf->csf ? esf->csf->fp : 0);
		EXCEPTION_DUMP("      t0: " PR_REG "      t1: " PR_REG, esf->t0, esf->t1);
		EXCEPTION_DUMP("      t2: " PR_REG "      t3: " PR_REG, esf->t2, esf->t3);
		EXCEPTION_DUMP("      t4: " PR_REG "      t5: " PR_REG, esf->t4, esf->t5);
		EXCEPTION_DUMP("      t6: " PR_REG "      t7: " PR_REG, esf->t6, esf->t7);
		EXCEPTION_DUMP("      t8: " PR_REG, esf->t8);
		EXCEPTION_DUMP("      a0: " PR_REG "      a1: " PR_REG, esf->a0, esf->a1);
		EXCEPTION_DUMP("      a2: " PR_REG "      a3: " PR_REG, esf->a2, esf->a3);
		EXCEPTION_DUMP("      a4: " PR_REG "      a5: " PR_REG, esf->a4, esf->a5);
		EXCEPTION_DUMP("      a6: " PR_REG "      a7: " PR_REG, esf->a6, esf->a7);
		EXCEPTION_DUMP("      sp: " PR_REG, z_loongarch_get_sp_before_exc(esf));
		EXCEPTION_DUMP("     era: " PR_REG, esf->era);
		EXCEPTION_DUMP("   estat: " PR_REG, esf->estat);
		EXCEPTION_DUMP("    crmd: " PR_REG, esf->crmd);
		EXCEPTION_DUMP("    badv: " PR_REG, esf->badv);
		EXCEPTION_DUMP("");

		csf = esf->csf;
	}

	if (csf != NULL) {
		EXCEPTION_DUMP("      s0: " PR_REG "      s1: " PR_REG, csf->s0, csf->s1);
		EXCEPTION_DUMP("      s2: " PR_REG "      s3: " PR_REG, csf->s2, csf->s3);
		EXCEPTION_DUMP("      s4: " PR_REG "      s5: " PR_REG, csf->s4, csf->s5);
		EXCEPTION_DUMP("      s6: " PR_REG "      s7: " PR_REG, csf->s6, csf->s7);
		EXCEPTION_DUMP("      s8: " PR_REG, csf->s8);
		EXCEPTION_DUMP("");
	} else if (esf != NULL) {
		EXCEPTION_DUMP("      s0: " PR_REG "      s1: " PR_REG, esf->s0, esf->s1);
		EXCEPTION_DUMP("      s2: " PR_REG "      s3: " PR_REG, esf->s2, esf->s3);
		EXCEPTION_DUMP("      s4: " PR_REG "      s5: " PR_REG, esf->s4, esf->s5);
		EXCEPTION_DUMP("      s6: " PR_REG "      s7: " PR_REG, esf->s6, esf->s7);
		EXCEPTION_DUMP("      s8: " PR_REG, esf->s8);
		EXCEPTION_DUMP("");
	}
#endif /* CONFIG_EXCEPTION_DEBUG */

#ifdef CONFIG_EXCEPTION_STACK_TRACE
	z_loongarch_unwind_stack(esf, csf);
#endif /* CONFIG_EXCEPTION_STACK_TRACE */

	_NanoFatalErrorHandler(reason, esf);
}

/* Fallback weak handler to satisfy link when arch impl is missing */
__weak void _NanoFatalErrorHandler(unsigned int reason, const struct arch_esf *esf)
{
	ARG_UNUSED(reason);
	ARG_UNUSED(esf);
	for (;;) {
		__asm__ volatile("idle 0");
	}
}

static bool bad_stack_pointer(struct arch_esf *esf)
{
#ifdef CONFIG_PMP_STACK_GUARD
	/*
	 * Check if the kernel stack pointer prior this exception (before
	 * storing the exception stack frame) was in the stack guard area.
	 */
	uintptr_t sp = (uintptr_t)esf + sizeof(struct arch_esf);

#ifdef CONFIG_USERSPACE
	if (_current->arch.priv_stack_start != 0 &&
	    sp >= _current->arch.priv_stack_start &&
	    sp <  _current->arch.priv_stack_start + Z_LOONGARCH_STACK_GUARD_SIZE) {
		return true;
	}

	if (z_stack_is_user_capable(_current->stack_obj) &&
	    sp >= _current->stack_info.start - K_THREAD_STACK_RESERVED &&
	    sp <  _current->stack_info.start - K_THREAD_STACK_RESERVED
		  + Z_LOONGARCH_STACK_GUARD_SIZE) {
		return true;
	}
#endif /* CONFIG_USERSPACE */

#if CONFIG_MULTITHREADING
	if (sp >= _current->stack_info.start - K_KERNEL_STACK_RESERVED &&
	    sp <  _current->stack_info.start - K_KERNEL_STACK_RESERVED
		  + Z_LOONGARCH_STACK_GUARD_SIZE) {
		return true;
	}
#else
	uintptr_t isr_stack = (uintptr_t)z_interrupt_stacks;
	uintptr_t main_stack = (uintptr_t)z_main_stack;

	if ((sp >= isr_stack && sp < isr_stack + Z_LOONGARCH_STACK_GUARD_SIZE) ||
	    (sp >= main_stack && sp < main_stack + Z_LOONGARCH_STACK_GUARD_SIZE)) {
		return true;
	}
#endif /* CONFIG_MULTITHREADING */
#endif /* CONFIG_PMP_STACK_GUARD */

#ifdef CONFIG_USERSPACE
	if ((esf->crmd & 0x3) == 3 &&
	    (esf->sp < _current->stack_info.start ||
	     esf->sp > _current->stack_info.start +
		       _current->stack_info.size -
		       _current->stack_info.delta)) {
		/* user stack pointer moved outside of its allowed stack */
		return true;
	}
#endif

	return false;
}

void z_loongarch_fault(struct arch_esf *esf)
{
	unsigned int reason = _NANO_ERR_CPU_EXCEPTION;

	if (bad_stack_pointer(esf)) {
#if defined(CONFIG_PMP_STACK_GUARD) && defined(CONFIG_MULTITHREADING)
		/*
		 * Remove the thread's PMP setting to prevent triggering a stack
		 * overflow error again due to the previous configuration.
		 */
#ifdef CONFIG_LOONGARCH_PMP
		z_loongarch_pmp_clear_all();
#endif
#endif /* CONFIG_PMP_STACK_GUARD */
		reason = _NANO_ERR_STACK_CHK_FAIL;
	}

	z_loongarch_fatal_error(reason, esf);
	CODE_UNREACHABLE;
}

#ifdef CONFIG_USERSPACE
FUNC_NORETURN void arch_syscall_oops(void *ssf_ptr)
{
	user_fault(K_ERR_KERNEL_OOPS);
	CODE_UNREACHABLE;
}

void z_impl_user_fault(unsigned int reason)
{
	struct arch_esf *oops_esf = _current->syscall_frame;

#ifdef CONFIG_EXCEPTION_DEBUG
	/* csf isn't populated in the syscall frame */
	if (oops_esf != NULL) {
		oops_esf->csf = NULL;
	}
#endif /* CONFIG_EXCEPTION_DEBUG */

	if (((_current->base.user_options & K_USER) != 0) &&
		reason != _NANO_ERR_STACK_CHK_FAIL) {
		reason = _NANO_ERR_KERNEL_OOPS;
	}
	z_loongarch_fatal_error(reason, oops_esf);
	CODE_UNREACHABLE;
}

static void z_vrfy_user_fault(unsigned int reason)
{
	z_impl_user_fault(reason);
}

#include <zephyr/syscalls/user_fault_mrsh.c>

#endif /* CONFIG_USERSPACE */
