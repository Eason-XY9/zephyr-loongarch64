/*
 * SPDX-License-Identifier: Apache-2.0
 */

/**
 * @file SoC configuration macros for LoongArch64 QEMU
 */

#ifndef __LOONGARCH64_QEMU_SOC_H_
#define __LOONGARCH64_QEMU_SOC_H_

#include <soc_common.h>
#include <misc/util.h>

/* UART configuration (placeholder for QEMU) */
#define LOONGARCH_QEMU_UART_BASE      0x900000001fe001e0UL
#define RISCV_QEMU_UART_BASE          LOONGARCH_QEMU_UART_BASE

/* Timer configuration (placeholder, align to QEMU virt timer if different) */
#define LOONGARCH_MTIME_BASE          0x40000000
#define LOONGARCH_MTIMECMP_BASE       0x40000008

/* lib-c hooks required RAM defined variables */
#define LOONGARCH_RAM_BASE            CONFIG_LOONGARCH_RAM_BASE_ADDR
#define LOONGARCH_RAM_SIZE            MB(CONFIG_LOONGARCH_RAM_SIZE_MB)

#endif /* __LOONGARCH64_QEMU_SOC_H_ */
