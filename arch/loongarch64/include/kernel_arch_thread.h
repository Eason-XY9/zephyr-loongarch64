/*
 * SPDX-License-Identifier: Apache-2.0
 */

/**
 * @file
 * @brief Per-arch thread definition for LOONGARCH64
 */

#ifndef _kernel_arch_thread__h_
#define _kernel_arch_thread__h_

#ifndef _ASMLANGUAGE
#include <zephyr/types.h>

struct _callee_saved {
	uintptr_t sp;    /* stack pointer */
	uintptr_t ra;    /* return address */
	uintptr_t a0;
	uintptr_t a1;
	uintptr_t a2;
	uintptr_t a3;
	uintptr_t a4;
	uintptr_t a5;
	uintptr_t a6;
	uintptr_t a7;
	uintptr_t t0;
	uintptr_t t1;
	uintptr_t t2;
	uintptr_t t3;
	uintptr_t t4;
	uintptr_t t5;
	uintptr_t t6;
	uintptr_t t7;
	uintptr_t t8;
    uintptr_t r21;
    uintptr_t fp;
	uintptr_t s0;
	uintptr_t s1;
	uintptr_t s2;
	uintptr_t s3;
	uintptr_t s4;
	uintptr_t s5;
	uintptr_t s6;
	uintptr_t s7;
	uintptr_t s8;
};
typedef struct _callee_saved _callee_saved_t;

struct _caller_saved {
	/* caller-saved registers are stacked in arch exception path */
};
typedef struct _caller_saved _caller_saved_t;

struct _thread_arch {
	uintptr_t swap_return_value; /* Return value of _Swap() */
#ifdef CONFIG_FPU_SHARING
	uint8_t exception_depth;
#endif
};
typedef struct _thread_arch _thread_arch_t;

#endif /* _ASMLANGUAGE */

#endif /* _kernel_arch_thread__h_ */
