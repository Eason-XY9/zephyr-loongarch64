/*
 * Copyright (c) 2024
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <irq.h>

#ifndef CONFIG_ARCH_HAS_CUSTOM_CPU_IDLE
void arch_cpu_idle(void)
{
	__asm__ volatile("idle 0");
	irq_unlock(CRMD_IEN);
}
#endif

#ifndef CONFIG_ARCH_HAS_CUSTOM_CPU_ATOMIC_IDLE
void arch_cpu_atomic_idle(unsigned int key)
{
	__asm__ volatile("idle 0");
	irq_unlock(key);
}
#endif

/* Kernel wrappers expected by common code */
void k_cpu_idle(void)
{
	arch_cpu_idle();
}

void k_cpu_atomic_idle(unsigned int key)
{
	arch_cpu_atomic_idle(key);
}
