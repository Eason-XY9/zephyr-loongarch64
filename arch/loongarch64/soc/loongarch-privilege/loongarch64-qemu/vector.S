/*
 * SPDX-License-Identifier: Apache-2.0
 */

#include <toolchain.h>
#include <linker/sections.h>

/* imports */
GTEXT(__reset)
GTEXT(__irq_wrapper)

/*
 * LoongArch QEMU vector table: place reset/NMI/MT vectors and jump
 * to __reset / __irq_wrapper.
 */
SECTION_FUNC(vectors, vinit)

	/* reset vector */
	b	__reset
	/* nmi vector */
	b	__irq_wrapper

	.org 0x10
	/* mt vector */
	b	__irq_wrapper
