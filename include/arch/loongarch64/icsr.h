/**
 * @file
 * @brief LOONGARCH indirect CSR (iCSR) access helpers with IRQ locking
 */

#ifndef ZEPHYR_INCLUDE_ARCH_LOONGARCH_ICSR_H_
#define ZEPHYR_INCLUDE_ARCH_LOONGARCH_ICSR_H_

#include <zephyr/arch/loongarch64/csr.h>
#include <zephyr/irq.h>

#ifdef CONFIG_LOONGARCH_ISA_EXT_SMCSRIND

/**
 * @brief Read an indirect CSR register atomically
 * @param index The CSR index to read
 * @return The value of the indirect CSR register
 */
static inline unsigned long csr_atomic_read(unsigned int csr)
{
    unsigned int key = irq_lock();

    unsigned long val = csr_read(csr);

    irq_unlock(key);

    return val;
}

/**
 * @brief Write an indirect CSR register atomically
 * @param index The CSR index to write
 * @param value The value to write to the indirect CSR register
 */
static inline void csr_atomic_write(unsigned int csr, unsigned long value)
{
    unsigned int key = irq_lock();

    csr_write(csr, value);

    irq_unlock(key);
}

/**
 * @brief Set bits in an indirect CSR register atomically
 * @param index The CSR index to modify
 * @param mask The bitmask of bits to set
 */
static inline void csr_atomic_set(unsigned int csr, unsigned long mask)
{
    unsigned int key = irq_lock();

    csr_set(csr, mask);

    irq_unlock(key);
}

/**
 * @brief Clear bits in an indirect CSR register atomically
 * @param index The CSR index to modify
 * @param mask The bitmask of bits to clear
 */
static inline void csr_atomic_clear(unsigned int csr, unsigned long mask)
{
    unsigned int key = irq_lock();

    csr_clear(csr, mask);

    irq_unlock(key);
}
/**
 * @brief Read an indirect CSR register and set bits atomically
 * @param index The CSR index to modify
 * @param mask The bitmask of bits to set
 * @return The previous value of the indirect CSR register
 */
static inline unsigned long csr_atomic_read_set(unsigned int csr, unsigned long mask)
{
    unsigned int key = irq_lock();

    unsigned long val = csr_read_set(csr, mask);

    irq_unlock(key);

    return val;
}

/**
 * @brief Read an indirect CSR register and clear bits atomically
 * @param index The CSR index to modify
 * @param mask The bitmask of bits to clear
 * @return The previous value of the indirect CSR register
 */
static inline unsigned long csr_atomic_read_clear(unsigned int csr, unsigned long mask)
{
    unsigned int key = irq_lock();

    unsigned long val = csr_read_clear(csr, mask);

    irq_unlock(key);

    return val;
}

/**
 * @brief Swap CSR atomically
 */
static inline unsigned long csr_atomic_swap(unsigned int csr, unsigned long val)
{
    unsigned int key = irq_lock();

    unsigned long old = csr_swap(csr, val);

    irq_unlock(key);

    return old;
}

#endif /* CONFIG_RISCV_ISA_EXT_SMCSRIND */

#endif /* ZEPHYR_INCLUDE_ARCH_LOONGARCH_ICSR_H_ */
