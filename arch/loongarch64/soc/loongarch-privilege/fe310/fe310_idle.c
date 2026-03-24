/*
 * SPDX-License-Identifier: Apache-2.0
 */

#include <toolchain.h>
#include <irq.h>
#include <soc.h>

#include <logging/kernel_event_logger.h>

static ALWAYS_INLINE void fe310_idle(unsigned int key)
{
#ifdef CONFIG_KERNEL_EVENT_LOGGER_SLEEP
	_sys_k_event_logger_enter_sleep();
#endif
	/* unlock interrupts */
	irq_unlock(key);

	/* Wait for interrupt */
	__asm__ volatile("idle 0");
}

void k_cpu_idle(void)
{
	fe310_idle(SOC_CRMD_IE);
}

void k_cpu_atomic_idle(unsigned int key)
{
	fe310_idle(key);
}
