/*
 * Copyright (c) 2024
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/**
 * @file
 * @brief LOONGARCH64 kernel structure member offset definition file
 *
 * This module is responsible for the generation of the absolute symbols whose
 * value represents the member offsets for various LOONGARCH64 kernel
 * structures.
 */

#include <arch/loongarch64/exception.h>
#include <arch/loongarch64/thread.h>
#include <kernel.h>
#include <kernel_arch_data.h>
#include <gen_offset.h>
#include <kernel_structs.h>

#ifdef CONFIG_LOONGARCH_SOC_CONTEXT_SAVE
#include <soc_context.h>
#endif
#ifdef CONFIG_LOONGARCH_SOC_OFFSETS
#include <soc_offsets.h>
#endif

#include <kernel_offsets.h>

/* thread_arch_t member offsets */
GEN_OFFSET_SYM(_thread_arch_t, swap_return_value);

/* struct _callee_saved member offsets */
GEN_OFFSET_SYM(_callee_saved_t, sp);
GEN_OFFSET_SYM(_callee_saved_t, ra);
GEN_OFFSET_SYM(_callee_saved_t, fp);
GEN_OFFSET_SYM(_callee_saved_t, s0);
GEN_OFFSET_SYM(_callee_saved_t, s1);
GEN_OFFSET_SYM(_callee_saved_t, s2);
GEN_OFFSET_SYM(_callee_saved_t, s3);
GEN_OFFSET_SYM(_callee_saved_t, s4);
GEN_OFFSET_SYM(_callee_saved_t, s5);
GEN_OFFSET_SYM(_callee_saved_t, s6);
GEN_OFFSET_SYM(_callee_saved_t, s7);
GEN_OFFSET_SYM(_callee_saved_t, s8);

#if defined(CONFIG_FPU_SHARING)

GEN_OFFSET_SYM(z_loongarch_fp_context_t, fa0);
GEN_OFFSET_SYM(z_loongarch_fp_context_t, fa1);
GEN_OFFSET_SYM(z_loongarch_fp_context_t, fa2);
GEN_OFFSET_SYM(z_loongarch_fp_context_t, fa3);
GEN_OFFSET_SYM(z_loongarch_fp_context_t, fa4);
GEN_OFFSET_SYM(z_loongarch_fp_context_t, fa5);
GEN_OFFSET_SYM(z_loongarch_fp_context_t, fa6);
GEN_OFFSET_SYM(z_loongarch_fp_context_t, fa7);

GEN_OFFSET_SYM(z_loongarch_fp_context_t, ft0);
GEN_OFFSET_SYM(z_loongarch_fp_context_t, ft1);
GEN_OFFSET_SYM(z_loongarch_fp_context_t, ft2);
GEN_OFFSET_SYM(z_loongarch_fp_context_t, ft3);
GEN_OFFSET_SYM(z_loongarch_fp_context_t, ft4);
GEN_OFFSET_SYM(z_loongarch_fp_context_t, ft5);
GEN_OFFSET_SYM(z_loongarch_fp_context_t, ft6);
GEN_OFFSET_SYM(z_loongarch_fp_context_t, ft7);
GEN_OFFSET_SYM(z_loongarch_fp_context_t, ft8);
GEN_OFFSET_SYM(z_loongarch_fp_context_t, ft9);
GEN_OFFSET_SYM(z_loongarch_fp_context_t, ft10);
GEN_OFFSET_SYM(z_loongarch_fp_context_t, ft11);
GEN_OFFSET_SYM(z_loongarch_fp_context_t, ft12);
GEN_OFFSET_SYM(z_loongarch_fp_context_t, ft13);
GEN_OFFSET_SYM(z_loongarch_fp_context_t, ft14);
GEN_OFFSET_SYM(z_loongarch_fp_context_t, ft15);

GEN_OFFSET_SYM(z_loongarch_fp_context_t, fs0);
GEN_OFFSET_SYM(z_loongarch_fp_context_t, fs1);
GEN_OFFSET_SYM(z_loongarch_fp_context_t, fs2);
GEN_OFFSET_SYM(z_loongarch_fp_context_t, fs3);
GEN_OFFSET_SYM(z_loongarch_fp_context_t, fs4);
GEN_OFFSET_SYM(z_loongarch_fp_context_t, fs5);
GEN_OFFSET_SYM(z_loongarch_fp_context_t, fs6);
GEN_OFFSET_SYM(z_loongarch_fp_context_t, fs7);

GEN_OFFSET_SYM(z_loongarch_fp_context_t, fcsr);
GEN_OFFSET_SYM(z_loongarch_fp_context_t, fcc);

GEN_OFFSET_SYM(_thread_arch_t, exception_depth);

#endif /* CONFIG_FPU_SHARING */

/* esf member offsets */
#ifndef CONFIG_LOONGARCH_SOC_HAS_ISR_STACKING
GEN_OFFSET_SYM(arch_esf_t, ra);
GEN_OFFSET_SYM(arch_esf_t, tp);

GEN_OFFSET_SYM(arch_esf_t, a0);
GEN_OFFSET_SYM(arch_esf_t, a1);
GEN_OFFSET_SYM(arch_esf_t, a2);
GEN_OFFSET_SYM(arch_esf_t, a3);
GEN_OFFSET_SYM(arch_esf_t, a4);
GEN_OFFSET_SYM(arch_esf_t, a5);
GEN_OFFSET_SYM(arch_esf_t, a6);
GEN_OFFSET_SYM(arch_esf_t, a7);

GEN_OFFSET_SYM(arch_esf_t, t0);
GEN_OFFSET_SYM(arch_esf_t, t1);
GEN_OFFSET_SYM(arch_esf_t, t2);
GEN_OFFSET_SYM(arch_esf_t, t3);
GEN_OFFSET_SYM(arch_esf_t, t4);
GEN_OFFSET_SYM(arch_esf_t, t5);
GEN_OFFSET_SYM(arch_esf_t, t6);
GEN_OFFSET_SYM(arch_esf_t, t7);
GEN_OFFSET_SYM(arch_esf_t, t8);

GEN_OFFSET_SYM(arch_esf_t, r21);
GEN_OFFSET_SYM(arch_esf_t, fp);

GEN_OFFSET_SYM(arch_esf_t, s0);
GEN_OFFSET_SYM(arch_esf_t, s1);
GEN_OFFSET_SYM(arch_esf_t, s2);
GEN_OFFSET_SYM(arch_esf_t, s3);
GEN_OFFSET_SYM(arch_esf_t, s4);
GEN_OFFSET_SYM(arch_esf_t, s5);
GEN_OFFSET_SYM(arch_esf_t, s6);
GEN_OFFSET_SYM(arch_esf_t, s7);
GEN_OFFSET_SYM(arch_esf_t, s8);

GEN_OFFSET_SYM(arch_esf_t, era);
GEN_OFFSET_SYM(arch_esf_t, estat);
GEN_OFFSET_SYM(arch_esf_t, crmd);
GEN_OFFSET_SYM(arch_esf_t, badv);

#if defined(CONFIG_SOC_LOONGARCH64_PULPINO)
GEN_OFFSET_SYM(NANO_ESF, lpstart0);
GEN_OFFSET_SYM(NANO_ESF, lpend0);
GEN_OFFSET_SYM(NANO_ESF, lpcount0);
GEN_OFFSET_SYM(NANO_ESF, lpstart1);
GEN_OFFSET_SYM(NANO_ESF, lpend1);
GEN_OFFSET_SYM(NANO_ESF, lpcount1);
#endif

#ifdef CONFIG_USERSPACE
GEN_OFFSET_SYM(arch_esf_t, sp);
#endif

#ifdef CONFIG_EXCEPTION_DEBUG
GEN_OFFSET_SYM(arch_esf_t, csf);
GEN_ABSOLUTE_SYM(__callee_saved_t_SIZEOF, ROUND_UP(sizeof(_callee_saved_t), ARCH_STACK_PTR_ALIGN));
#endif /* CONFIG_EXCEPTION_DEBUG */

#if defined(CONFIG_LOONGARCH_SOC_CONTEXT_SAVE)
GEN_OFFSET_SYM(arch_esf_t, soc_context);
#endif

GEN_ABSOLUTE_SYM(__struct_arch_esf_SIZEOF, sizeof(arch_esf_t));

#endif /* CONFIG_LOONGARCH_SOC_HAS_ISR_STACKING */

#if defined(CONFIG_LOONGARCH_SOC_OFFSETS)
GEN_SOC_OFFSET_SYMS();
#endif

#ifdef CONFIG_USERSPACE
/* TODO: add userspace-specific CPU arch data when implemented */
#endif

GEN_ABS_SYM_END
