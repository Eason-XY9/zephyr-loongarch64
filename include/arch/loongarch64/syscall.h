/*
 * Copyright (c) 2024
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/**
 * @file
 * @brief LOONGARCH specific syscall header
 *
 * This header contains the LOONGARCH specific syscall interface.  It is
 * included by the syscall interface architecture-abstraction header
 * (include/arch/syscall.h)
 */

#ifndef ZEPHYR_INCLUDE_ARCH_LOONGARCH_SYSCALL_H_
#define ZEPHYR_INCLUDE_ARCH_LOONGARCH_SYSCALL_H_

/*
 * Privileged mode system calls
 */
#define LARCH_SYSCALL_RUNTIME_EXCEPT		0
#define LARCH_SYSCALL_IRQ_OFFLOAD		1
#define LARCH_SYSCALL_SCHEDULE			2

#ifndef _ASMLANGUAGE

#include <zephyr/types.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/*
 * Syscall invocation macros. loongarch-specific machine constraints used to ensure
 * args land in the proper registers.
 * In LoongArch, arguments are passed in a0-a7. We use a7 for call_id to align
 * with the standard calling convention.
 */
static inline uintptr_t arch_syscall_invoke6(uintptr_t arg1, uintptr_t arg2,
					     uintptr_t arg3, uintptr_t arg4,
					     uintptr_t arg5, uintptr_t arg6,
					     uintptr_t call_id)
{
	register unsigned long a0 __asm__ ("a0") = arg1;
	register unsigned long a1 __asm__ ("a1") = arg2;
	register unsigned long a2 __asm__ ("a2") = arg3;
	register unsigned long a3 __asm__ ("a3") = arg4;
	register unsigned long a4 __asm__ ("a4") = arg5;
	register unsigned long a5 __asm__ ("a5") = arg6;
	register unsigned long a7 __asm__ ("a7") = call_id;

	__asm__ volatile ("syscall 0"
			  : "+r" (a0)
			  : "r" (a1), "r" (a2), "r" (a3), "r" (a4), "r" (a5),
			  "r" (a7)
			  : "memory");
	return a0;
}

static inline uintptr_t arch_syscall_invoke5(uintptr_t arg1, uintptr_t arg2,
					     uintptr_t arg3, uintptr_t arg4,
					     uintptr_t arg5,
					     uintptr_t call_id)
{
	register unsigned long a0 __asm__ ("a0") = arg1;
	register unsigned long a1 __asm__ ("a1") = arg2;
	register unsigned long a2 __asm__ ("a2") = arg3;
	register unsigned long a3 __asm__ ("a3") = arg4;
	register unsigned long a4 __asm__ ("a4") = arg5;
	register unsigned long a7 __asm__ ("a7") = call_id;

	__asm__ volatile ("syscall 0"
			  : "+r" (a0)
			  : "r" (a1), "r" (a2), "r" (a3), "r" (a4), "r" (a7)
			  : "memory");
	return a0;
}

static inline uintptr_t arch_syscall_invoke4(uintptr_t arg1, uintptr_t arg2,
					     uintptr_t arg3, uintptr_t arg4,
					     uintptr_t call_id)
{
	register unsigned long a0 __asm__ ("a0") = arg1;
	register unsigned long a1 __asm__ ("a1") = arg2;
	register unsigned long a2 __asm__ ("a2") = arg3;
	register unsigned long a3 __asm__ ("a3") = arg4;
	register unsigned long a7 __asm__ ("a7") = call_id;

	__asm__ volatile ("syscall 0"
			  : "+r" (a0)
			  : "r" (a1), "r" (a2), "r" (a3), "r" (a7)
			  : "memory");
	return a0;
}

static inline uintptr_t arch_syscall_invoke3(uintptr_t arg1, uintptr_t arg2,
					     uintptr_t arg3,
					     uintptr_t call_id)
{
	register unsigned long a0 __asm__ ("a0") = arg1;
	register unsigned long a1 __asm__ ("a1") = arg2;
	register unsigned long a2 __asm__ ("a2") = arg3;
	register unsigned long a7 __asm__ ("a7") = call_id;

	__asm__ volatile ("syscall 0"
			  : "+r" (a0)
			  : "r" (a1), "r" (a2), "r" (a7)
			  : "memory");
	return a0;
}

static inline uintptr_t arch_syscall_invoke2(uintptr_t arg1, uintptr_t arg2,
					     uintptr_t call_id)
{
	register unsigned long a0 __asm__ ("a0") = arg1;
	register unsigned long a1 __asm__ ("a1") = arg2;
	register unsigned long a7 __asm__ ("a7") = call_id;

	__asm__ volatile ("syscall 0"
			  : "+r" (a0)
			  : "r" (a1), "r" (a7)
			  : "memory");
	return a0;
}

static inline uintptr_t arch_syscall_invoke1(uintptr_t arg1, uintptr_t call_id)
{
	register unsigned long a0 __asm__ ("a0") = arg1;
	register unsigned long a7 __asm__ ("a7") = call_id;

	__asm__ volatile ("syscall 0"
			  : "+r" (a0)
			  : "r" (a7)
			  : "memory");
	return a0;
}

static inline uintptr_t arch_syscall_invoke0(uintptr_t call_id)
{
	register unsigned long a0 __asm__ ("a0");
	register unsigned long a7 __asm__ ("a7") = call_id;

	__asm__ volatile ("syscall 0"
			  : "=r" (a0)
			  : "r" (a7)
			  : "memory");
	return a0;
}

#ifdef CONFIG_USERSPACE
register unsigned long loongarch_tp_reg __asm__ ("tp");

static inline bool arch_is_user_context(void)
{
	/* don't try accessing TLS variables if tp is not initialized */
	if (loongarch_tp_reg == 0) {
		return false;
	}

	/* Defined in arch/loongarch/core/thread.c */
	extern Z_THREAD_LOCAL uint8_t is_user_mode;

	return is_user_mode != 0;
}
#endif

#ifdef __cplusplus
}
#endif

#endif /* _ASMLANGUAGE */
#endif /* ZEPHYR_INCLUDE_ARCH_LOONGARCH_SYSCALL_H_ */