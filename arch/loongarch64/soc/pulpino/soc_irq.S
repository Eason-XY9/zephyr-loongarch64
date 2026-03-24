/*
 * Copyright (c) 2016 Jean-Paul Etienne <fractalclone@gmail.com>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <kernel_structs.h>
#include <offsets.h>
#include <toolchain.h>
#include <linker/sections.h>
#include <soc.h>

/* exports */
GTEXT(__soc_save_context)
GTEXT(__soc_restore_context)
GTEXT(__soc_is_irq)
GTEXT(__soc_handle_irq)
GTEXT(__soc_irq_unlock)

/* Use ABI name of registers for the sake of simplicity */

/*
 * Pulpino core has hardware loops registers that need to be saved
 * prior to handling an interrupt/exception.
 *
 * NOTE: Stack space allocation is not needed here, as already allocated at
 * architecture level with __NANO_ESF_SIZEOF value (including space for the
 * pulpino-specific registers)
 */
SECTION_FUNC(exception.other, __soc_save_context)
	/* Save hardware loop registers to stack */
	csrrd 	$t0, PULP_LPSTART0
	csrrd 	$t1, PULP_LPEND0
	csrrd 	$t2, PULP_LPCOUNT0
	st.d 	$t0, $sp, __NANO_ESF_lpstart0_OFFSET
	st.d 	$t1, $sp, __NANO_ESF_lpend0_OFFSET
	st.d 	$t2, $sp, __NANO_ESF_lpcount0_OFFSET
	csrrd 	$t0, PULP_LPSTART1
	csrrd 	$t1, PULP_LPEND1
	csrrd 	$t2, PULP_LPCOUNT1
	st.d 	$t0, $sp, __NANO_ESF_lpstart1_OFFSET
	st.d 	$t1, $sp, __NANO_ESF_lpend1_OFFSET
	st.d 	$t2, $sp, __NANO_ESF_lpcount1_OFFSET

	/* Return */
	jirl $zero, $ra, 0


SECTION_FUNC(exception.other, __soc_restore_context)
	/* Restore hardloop registers from stack */
	ld.d 	$t0, $sp, __NANO_ESF_lpstart0_OFFSET
	ld.d 	$t1, $sp, __NANO_ESF_lpend0_OFFSET
	ld.d 	$t2, $sp, __NANO_ESF_lpcount0_OFFSET
	csrwr   $t0, PULP_LPSTART0
	csrwr   $t1, PULP_LPEND0
	csrwr 	$t2, PULP_LPCOUNT0
	ld.d 	$t0, $sp, __NANO_ESF_lpstart1_OFFSET
	ld.d 	$t1, $sp, __NANO_ESF_lpend1_OFFSET
	ld.d 	$t2, $sp, __NANO_ESF_lpcount1_OFFSET
	csrwr  	$t0, PULP_LPSTART1
	csrwr 	$t1, PULP_LPEND1
	csrwr 	$t2, PULP_LPCOUNT1

	/* Return */
	jirl 	$zero, $ra, 0


/*
 * SOC-specific function to handle pending IRQ number generating the interrupt.
 *
 * The pulpino core has:
 * 1) an ICP register, which is used to clear the pending
 *    IRQ number upon an interrupt.
 * 2) an ECP register, which is used to clear the pending IRQ number
 *    that has woken up the CPU from sleep state.
 *
 * Exception number is given as parameter via register a0.
 */
SECTION_FUNC(exception.other, __soc_handle_irq)
	/* Return */
	jirl 	$zero, $ra, 0


/*
 * SOC-specific function to determine if the exception is the result of a
 * an interrupt or an exception
 * return 1 (interrupt) or 0 (exception)
 */
SECTION_FUNC(exception.other, __soc_is_irq)
	/* Get exception number from the mcause CSR register. */
	csrrd 	$t0, CSR_ESTAT
	andi.d	$t1, $t0, 0x1fff

	move	$a0, $zero

	beqz 	$t1, not_interrupt

	li 		$a0, 1
not_interrupt:
	/* return */
	jirl 	$zero, $ra, 0
