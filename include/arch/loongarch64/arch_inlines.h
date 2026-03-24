#ifndef ZEPHYR_INCLUDE_ARCH_LOONGARCH_ARCH_INLINES_H_
#define ZEPHYR_INCLUDE_ARCH_LOONGARCH_ARCH_INLINES_H_

#ifndef _ASMLANGUAGE

#include "csr.h"

static ALWAYS_INLINE uint32_t arch_proc_id(void)
{
    return csr_read(CSR_CPUID);
}

#ifdef CONFIG_LOONGARCH_CURRENT_VIA_GP

register struct k_thread *__arch_current_thread __asm__("gp");

#define arch_current_thread() __arch_current_thread
#define arch_current_thread_set(thread) \
    ({ __arch_current_thread = (thread); })

#endif /* CONFIG_LOONGARCH_CURRENT_VIA_GP */

static ALWAYS_INLINE unsigned int arch_num_cpus(void)
{
    return 1U;
}

#endif /* !_ASMLANGUAGE */

#endif /* ZEPHYR_INCLUDE_ARCH_LOONGARCH_ARCH_INLINES_H_ */