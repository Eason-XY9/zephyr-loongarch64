/*
 * SPDX-License-Identifier: Apache-2.0
 */

#include <toolchain.h>
#include <linker/sections.h>

/* imports */
GTEXT(__start)
GTEXT(__irq_wrapper)

SECTION_FUNC(vectors, vinit)
	.option push
	.option norelax

	/* Set EENTRY to __irq_wrapper */
	la.pcrel	$t0, __irq_wrapper
	csrw	EENTRY, $t0

	/* Jump to __start */
	jirl	$zero, __start, 0

	.option pop
