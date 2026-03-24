/*
 * Copyright (c) 2024
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <irq_offload.h>
#include <arch/loongarch64/syscall.h>

void arch_irq_offload(irq_offload_routine_t routine, const void *parameter)
{
	arch_syscall_invoke2((uintptr_t)routine, (uintptr_t)parameter, LARCH_SYSCALL_IRQ_OFFLOAD);
}

void arch_irq_offload_init(void)
{
}
