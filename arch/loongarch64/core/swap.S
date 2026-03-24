/*
 * SPDX-License-Identifier: Apache-2.0
 */

#include <irq.h>
#include <kernel_structs.h>
#include <offsets_short.h>
#include <arch/loongarch64/csr.h>
#include <arch/loongarch64/irq.h>
#include <arch/loongarch64/syscall.h>

/* exports */
GTEXT(__swap)
GTEXT(_thread_entry_wrapper)

/*
 * unsigned int __swap(unsigned int key)
 *
 * Always called with interrupts locked
 * key is stored in a0 register
 */
SECTION_FUNC(exception.other, __swap)

	/* trigger a syscall to request a reschedule */
	li.d	    $a7, LARCH_SYSCALL_SCHEDULE
	syscall	    0

	/* when we are rescheduled, restore key-dependent IRQ state */
	la.pcrel	$t0, _kernel
	ld.d	    $t1, $t0, _kernel_offset_to_current
	ld.d	    $t2, $t1, _thread_offset_to_swap_return_value

	/* restore IE bit according to saved key */
	li.d	    $t3, CRMD_IE
	and	        $t4, $a0, $t3
	csrxchg	    $t4, $t3, CSR_CRMD

	/* return swap return value */
	move	    $a0, $t2
	jirl	    $zero, $ra, 0

/*
 * void _thread_entry_wrapper(_thread_entry_t, void *, void *, void *)
 */
SECTION_FUNC(TEXT, _thread_entry_wrapper)
	la.pcrel	$t0, _thread_entry
	jirl	    $zero, $t0, 0
