/*
 * SPDX-License-Identifier: Apache-2.0
 */

#include <irq.h>
#include <arch/loongarch64/csr.h>

void _arch_irq_enable(unsigned int irq)
{
	unsigned long mask = 1UL << irq;
	csr_set(CSR_ECFG, mask);
}

void _arch_irq_disable(unsigned int irq)
{
	unsigned long mask = 1UL << irq;
	csr_clear(CSR_ECFG, mask);
}

int _arch_irq_is_enabled(unsigned int irq)
{
	unsigned long mask = 1UL << irq;
	return (csr_read(CSR_ECFG) & mask) != 0UL;
}

#if defined(CONFIG_LOONGARCH_SOC_INTERRUPT_INIT)
void soc_interrupt_init(void)
{
	(void)irq_lock();
	csr_write(CSR_ECFG, 0UL);
	csr_write(CSR_ESTAT, 0UL);
}
#endif
