/*
 * Copyright (c) 2024
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <kernel.h>
#include <misc/printk.h>
#include <arch/loongarch64/csr.h>
#include <arch/loongarch64/irq.h>
#include <arch/loongarch64/exception.h>
#include <sw_isr_table.h>
#include <device.h>

extern void z_loongarch_fatal_error(unsigned int reason, const arch_esf_t *esf);

FUNC_NORETURN void z_irq_spurious(const void *unused)
{
#ifdef CONFIG_EMPTY_IRQ_SPURIOUS
	while (1) {
	}

	CODE_UNREACHABLE;
#else
	unsigned long estat;

	ARG_UNUSED(unused);

	estat = csr_read(CSR_ESTAT);

	estat &= LOONGARCH_ESTAT_IS_MASK;

	printk("Spurious interrupt detected! IRQ: %ld\n", estat);
	z_loongarch_fatal_error(_NANO_ERR_SPURIOUS_INT, NULL);
	CODE_UNREACHABLE;
#endif /* CONFIG_EMPTY_IRQ_SPURIOUS */
}

void _irq_spurious(void *unused)
{
	z_irq_spurious(unused);
}

#ifdef CONFIG_DYNAMIC_INTERRUPTS
int arch_irq_connect_dynamic(unsigned int irq, unsigned int priority,
			     void (*routine)(const void *parameter),
			     const void *parameter, uint32_t flags)
{
	z_isr_install(irq, routine, parameter);

	ARG_UNUSED(flags);
	ARG_UNUSED(priority);

	return irq;
}

#ifdef CONFIG_SHARED_INTERRUPTS
int arch_irq_disconnect_dynamic(unsigned int irq, unsigned int priority,
				void (*routine)(const void *parameter), const void *parameter,
				uint32_t flags)
{
	ARG_UNUSED(priority);
	ARG_UNUSED(flags);

	return z_isr_uninstall(irq, routine, parameter);
}
#endif /* CONFIG_SHARED_INTERRUPTS */
#endif /* CONFIG_DYNAMIC_INTERRUPTS */

/* Provide clock driver init stub when none is linked */
__weak int _sys_clock_driver_init(struct device *dev)
{
	ARG_UNUSED(dev);
	return 0;
}

