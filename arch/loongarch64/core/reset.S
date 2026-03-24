/*
 * Copyright (c) 2016 Jean-Paul Etienne <fractalclone@gmail.com>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <kernel_structs.h>

/* exports */
GTEXT(__start)
GTEXT(__reset)

/* imports */
GTEXT(_PrepC)

#if CONFIG_INCLUDE_RESET_VECTOR
SECTION_FUNC(reset, __reset)
	/*
	 * jump to __start
	 * use call opcode in case __start is far away.
	 * This will be dependent on linker.ld configuration.
	 */
	bl __start
#endif /* CONFIG_INCLUDE_RESET_VECTOR */

/* use ABI name of registers for the sake of simplicity */

/*
 * Remainder of asm-land initialization code before we can jump into
 * the C domain
 */
SECTION_FUNC(TEXT, __start)
#ifdef CONFIG_INIT_STACKS

    li.d    $t0, 0x9000000000000011
    csrwr   $t0, csr_dmw0
    li.d    $t0, 0x0000000000000000
    csrwr   $t0, csr_dmw1
    csrwr   $t0, csr_dmw2
    csrwr   $t0, csr_dmw3

	/* Pre-populate all bytes in _interrupt_stack with 0xAA */
	la      $t0, _interrupt_stack
	la      $t1, CONFIG_ISR_STACK_SIZE
	add.d   $t1, $t1, $t0

	/* Populate _interrupt_stack with 0xaaaaaaaa */
	li.d    $t2, 0xaaaaaaaaaaaaaaaa
aa_loop:
	st.w 	$t2, $t0, 0
	addi.d 	$t0, $t0, 4
	blt 	$t0, $t1, aa_loop
#endif

	/*
	 * Initially, setup stack pointer to
	 * _interrupt_stack + CONFIG_ISR_STACK_SIZE
	 */
	la 		$sp, _interrupt_stack
	li.d 	$t0, CONFIG_ISR_STACK_SIZE
	add.d 	$sp, $sp, $t0

	/*
	 * Jump into C domain. _PrepC zeroes BSS, copies rw data into RAM,
	 * and then enters kernel _Cstart
	 */
	bl _PrepC
