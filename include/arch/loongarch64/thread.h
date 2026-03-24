/*
 * Copyright (c) 2024
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/**
 * @file
 * @brief Per-arch thread definition
 *
 * This file contains definitions for
 *
 *  struct _thread_arch
 *  struct _callee_saved
 *
 * necessary to instantiate instances of struct k_thread.
 */

#ifndef ZEPHYR_INCLUDE_ARCH_LOONGARCH_THREAD_H_
#define ZEPHYR_INCLUDE_ARCH_LOONGARCH_THREAD_H_

#ifndef _ASMLANGUAGE
#include <zephyr/types.h>
#include <arch/loongarch64/csr.h>
#include <arch/loongarch64/syscall.h>
#include <kernel_arch_thread.h>

/* FP context definition (callee/thread structs already in kernel_arch_thread.h) */
#if !defined(LA_FP_TYPE)
#ifdef CONFIG_CPU_HAS_FPU_DOUBLE_PRECISION
#define LA_FP_TYPE uint64_t
#else
#define LA_FP_TYPE uint32_t
#endif
#endif

struct z_loongarch_fp_context {
	LA_FP_TYPE fa0, fa1, fa2, fa3, fa4, fa5, fa6, fa7;
	LA_FP_TYPE ft0, ft1, ft2, ft3, ft4, ft5, ft6, ft7, ft8, ft9, ft10, ft11, ft12, ft13, ft14, ft15;
	LA_FP_TYPE fs0, fs1, fs2, fs3, fs4, fs5, fs6, fs7;
	uint32_t fcsr;
	uint32_t fcc;
};
typedef struct z_loongarch_fp_context z_loongarch_fp_context_t;

#endif /* _ASMLANGUAGE */

#endif /* ZEPHYR_INCLUDE_ARCH_LOONGARCH_THREAD_H_ */