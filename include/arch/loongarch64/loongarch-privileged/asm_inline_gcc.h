/*
 * Copyright (c) 2017 Jean-Paul Etienne <fractalclone@gmail.com>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef ZEPHYR_INCLUDE_ARCH_LOONGARCH_LOONGARCH_PRIVILEGED_ASM_INLINE_GCC_H_
#define ZEPHYR_INCLUDE_ARCH_LOONGARCH_LOONGARCH_PRIVILEGED_ASM_INLINE_GCC_H_

/*
 * The file must not be included directly
 * Include arch/cpu.h instead
 */

#ifndef _ASMLANGUAGE

#include <toolchain.h>
#include <zephyr/types.h>

/* Bit operations */
static ALWAYS_INLINE unsigned int find_lsb_set(uint32_t op)
{
	return __builtin_ffs(op);
}

static ALWAYS_INLINE unsigned int find_msb_set(uint32_t op)
{
	if (!op) {
		return 0U;
	}

	return 32U - __builtin_clz(op);
}

/* Barriers */
static ALWAYS_INLINE void arch_mem_barrier(void)
{
	__asm__ volatile ("dbar 0" : : : "memory");
}

static ALWAYS_INLINE void arch_instr_barrier(void)
{
	__asm__ volatile ("ibar 0" : : : "memory");
}

#endif /* _ASMLANGUAGE */

#endif /* ZEPHYR_INCLUDE_ARCH_LOONGARCH_LOONGARCH_PRIVILEGED_ASM_INLINE_GCC_H_ */
