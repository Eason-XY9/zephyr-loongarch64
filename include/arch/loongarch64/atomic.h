/**
 * Copyright (c) 2026
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef ZEPHYR_INCLUDE_ARCH_LOONGARCH_ATOMIC_H_
#define ZEPHYR_INCLUDE_ARCH_LOONGARCH_ATOMIC_H_

#ifdef __cplusplus
extern "C" {
#endif

/*
 * LoongArch-specific atomics supplementing atomic_builtin.h.
 *
 * Keep API/signatures aligned with loongarch/atomic.h.
 */

#ifdef CONFIG_64BIT

static ALWAYS_INLINE atomic_val_t atomic_swap(const atomic_t *target, atomic_val_t newval)
{
	atomic_val_t ret;

	__asm__ volatile("amoswap.d  %0, %1, %2"
			 : "=r"(ret)
			 : "r"(newval), "A"(*target)
			 : "memory");

	return ret;
}

static ALWAYS_INLINE atomic_val_t atomic_max(atomic_t *target, atomic_val_t value)
{
	atomic_val_t ret;

	__asm__ volatile("ammax_db.d  %0, %1, %2"
			 : "=r"(ret)
			 : "r"(value), "A"(*target)
			 : "memory");

	return ret;
}

static ALWAYS_INLINE atomic_val_t atomic_min(atomic_t *target, atomic_val_t value)
{
	atomic_val_t ret;

	__asm__ volatile("ammin_db.d  %0, %1, %2"
			 : "=r"(ret)
			 : "r"(value), "A"(*target)
			 : "memory");

	return ret;
}

static ALWAYS_INLINE atomic_val_t atomic_maxu(unsigned long *target, unsigned long value)
{
	unsigned long ret;

	__asm__ volatile("ammax_db.du  %0, %1, %2"
			 : "=r"(ret)
			 : "r"(value), "A"(*target)
			 : "memory");

	return ret;
}

static ALWAYS_INLINE atomic_val_t atomic_minu(unsigned long *target, unsigned long value)
{
	unsigned long ret;

	__asm__ volatile("ammin_db.du  %0, %1, %2"
			 : "=r"(ret)
			 : "r"(value), "A"(*target)
			 : "memory");

	return ret;
}

#else

static ALWAYS_INLINE atomic_val_t atomic_swap(const atomic_t *target, atomic_val_t newval)
{
	atomic_val_t ret;

	__asm__ volatile("amswap_db.w  %0, %1, %2"
			 : "=r"(ret)
			 : "r"(newval), "A"(*target)
			 : "memory");

	return ret;
}

static ALWAYS_INLINE atomic_val_t atomic_max(atomic_t *target, atomic_val_t value)
{
	atomic_val_t ret;

	__asm__ volatile("ammax_db.w  %0, %1, %2"
			 : "=r"(ret)
			 : "r"(value), "A"(*target)
			 : "memory");

	return ret;
}

static ALWAYS_INLINE atomic_val_t atomic_min(atomic_t *target, atomic_val_t value)
{
	atomic_val_t ret;

	__asm__ volatile("ammin_db.w  %0, %1, %2"
			 : "=r"(ret)
			 : "r"(value), "A"(*target)
			 : "memory");

	return ret;
}

static ALWAYS_INLINE unsigned long atomic_maxu(unsigned long *target, unsigned long value)
{
	unsigned long ret;

	__asm__ volatile("ammax_db.wu  %0, %1, %2"
			 : "=r"(ret)
			 : "r"(value), "A"(*target)
			 : "memory");

	return ret;
}

static ALWAYS_INLINE unsigned long atomic_minu(unsigned long *target, unsigned long value)
{
	unsigned long ret;

	__asm__ volatile("ammin_db.wu  %0, %1, %2"
			 : "=r"(ret)
			 : "r"(value), "A"(*target)
			 : "memory");

	return ret;
}

#endif /* CONFIG_64BIT */

#ifdef __cplusplus
}
#endif

#endif /* ZEPHYR_INCLUDE_ARCH_LOONGARCH_ATOMIC_H_ */
