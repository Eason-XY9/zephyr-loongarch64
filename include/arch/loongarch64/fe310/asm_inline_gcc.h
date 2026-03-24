/*
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef _ASM_INLINE_GCC_H
#define _ASM_INLINE_GCC_H

#ifdef __cplusplus
extern "C" {
#endif

#ifndef _ASMLANGUAGE

#include <toolchain.h>
#include <zephyr/types.h>

/*
 * Inline helpers for bit scan operations on LOONGARCH64.
 */
static ALWAYS_INLINE unsigned int find_lsb_set(u32_t op)
{
	return __builtin_ffs(op);
}

static ALWAYS_INLINE unsigned int find_msb_set(u32_t op)
{
	if (!op) {
		return 0;
	}

	return 32 - __builtin_clz(op);
}

#endif /* _ASMLANGUAGE */

#ifdef __cplusplus
}
#endif

#endif /* _ASM_INLINE_GCC_H */
