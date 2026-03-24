/*
 * SPDX-License-Identifier: Apache-2.0
 */

#include <kernel_structs.h>
#include <offsets.h>
#include <toolchain.h>
#include <linker/sections.h>
#include <soc.h>

/* exports */
GTEXT(__soc_handle_irq)
GTEXT(__soc_is_irq)

/*
 * Clear pending bit for given IRQ in ESTAT.
 * a0 contains irq number (0..12 for built-ins).
 */
SECTION_FUNC(exception.other, __soc_handle_irq)
	li.d   $t1, 1
	sll.d  $t0, $t1, $a0
	csrxchg $zero, $t0, CSR_ESTAT  /* clear pending bit */
	jirl   $zero, $ra, 0

/*
 * Determine if current exception is an interrupt.
 * Return 1 in a0 if interrupt, else 0.
 */
SECTION_FUNC(exception.other, __soc_is_irq)
	csrrd  	$t0, CSR_ESTAT
	li.d	$t1, LOONGARCH_ESTAT_ECODE_MASK
	and		$t0, $t0, $t1
	li.d   	$t1, (LOONGARCH_EXC_INT << LOONGARCH_ESTAT_ECODE_SHIFT)
	move   	$a0, $zero
	bne    	$t0, $t1, 1f
	addi.d 	$a0, $a0, 1
1:
	jirl   $zero, $ra, 0
