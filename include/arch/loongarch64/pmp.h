/*
 * Copyright (c) 2026
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef ZEPHYR_INCLUDE_LOONGARCH_PMP_H_
#define ZEPHYR_INCLUDE_LOONGARCH_PMP_H_

#include <zephyr/arch/loongarch64/arch.h>
#include <zephyr/sys/iterable_sections.h>

#define LA_MPU_R   (1 << 0)
#define LA_MPU_W   (1 << 1)
#define LA_MPU_X   (1 << 2)

struct la_mpu_region {
    uintptr_t start;
    uintptr_t size;
    uint8_t perm;
};

/* 最大区域数（由硬件决定） */
#define CONFIG_LA_MPU_SLOTS 16

struct la_mpu_state {
    struct la_mpu_region regions[CONFIG_LA_MPU_SLOTS];
    unsigned int count;
};

#endif