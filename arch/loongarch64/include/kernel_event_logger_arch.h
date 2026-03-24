/*
 * SPDX-License-Identifier: Apache-2.0
 */

/**
 * @file
 * @brief Kernel event logger support for LOONGARCH64
 */

#ifndef __KERNEL_EVENT_LOGGER_ARCH_H__
#define __KERNEL_EVENT_LOGGER_ARCH_H__

#include <arch/cpu.h>
#include <zephyr/arch/loongarch64/csr.h>

#ifdef __cplusplus
extern "C" {
#endif

static inline int _sys_current_irq_key_get(void)
{
	/* ESTAT encodes pending IRQ in IS[12:0]; extract current vector */
	unsigned long estat = csr_read(CSR_ESTAT);
	return (int)(estat & LOONGARCH_ESTAT_IS_MASK);
}

#ifdef __cplusplus
}
#endif

#endif /* __KERNEL_EVENT_LOGGER_ARCH_H__ */
