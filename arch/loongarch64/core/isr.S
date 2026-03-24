#include <toolchain.h>
#include <linker/sections.h>
#include <offsets_short.h>
#include <arch/loongarch64/csr.h>
#include <arch/loongarch64/irq.h>
#include <arch/loongarch64/syscall.h>
#include "asm_macros.inc"

#define ESF_STACK_SIZE __struct_arch_esf_SIZEOF
#define ESF_ERA_OFFSET __arch_esf_t_era_OFFSET

/* imports */
GDATA(_sw_isr_table)
GTEXT(__soc_handle_irq)
GTEXT(z_loongarch_fault)
GTEXT(z_irq_spurious)

#ifdef CONFIG_LOONGARCH_SOC_CONTEXT_SAVE
GTEXT(__soc_save_context)
GTEXT(__soc_restore_context)
#endif /* CONFIG_LOONGARCH_SOC_CONTEXT_SAVE */

GTEXT(z_loongarch_fatal_error)
GTEXT(z_get_next_switch_handle)
GTEXT(z_loongarch_switch)
GTEXT(z_loongarch_thread_start)

#ifdef CONFIG_TRACING
GTEXT(sys_trace_isr_enter)
GTEXT(sys_trace_isr_exit)
#endif

#ifdef CONFIG_USERSPACE
GDATA(_k_syscall_table)
#endif

#ifdef CONFIG_LOONGARCH_SOC_HAS_CUSTOM_IRQ_HANDLING
GTEXT(__soc_handle_all_irqs)
#endif

/* exports */
GTEXT(_isr_wrapper)


SECTION_FUNC(exception.entry, __irq_wrapper)
	
	addi.d  $sp, $sp, -ESF_STACK_SIZE
	
	store_load_regs st.d

	csrrd   $t0, CSR_ERA
    st.d    $t0, $sp, ESF_ERA_OFFSET

	csrrd   $t0, CSR_PRMD
    csrwr   $t0, CSR_SAVE1

#ifdef CONFIG_LOONGARCH_SOC_CONTEXT_SAVE
	bl 		__soc_save_context
#endif

	bl 		__soc_is_irq

	bnez 	$a0, is_interrupt

	csrrd 	$t0, CSR_ESTAT
	li.d	$t1, LOONGARCH_ESTAT_ECODE_MASK
	and	$t0, $t0, $t1

	li.d    $t1, (LOONGARCH_EXC_SYS << LOONGARCH_ESTAT_ECODE_SHIFT)

#ifdef CONFIG_IRQ_OFFLOAD
	bne 	$t0, $t1, is_fault

#else
	beq		$t0, $t1, reschedule
#endif

is_fault:

	addi.d 	$a0, $sp, 0
    bl     	z_loongarch_fatal_error
    b      	no_reschedule

is_interrupt:

	csrrd	$a0, CSR_ESTAT
	li.d	$t1, LOONGARCH_ESTAT_IS_MASK
	and	    $a0, $a0, $t1


	bl      __soc_handle_irq

	la 		$t0, _sw_isr_table

    slli.d  $t1, $a0, 4
    add.d   $t0, $t0, $t1

    ld.d    $a0, $t0, 0
    ld.d    $t1, $t0, 8


    jirl    $ra, $t1, 0

    b       no_reschedule

reschedule:
#if CONFIG_TIMESLICING
	bl		_update_time_slice_before_swap
#endif
#if CONFIG_KERNEL_EVENT_LOGGER_CONTEXT_SWITCH
	bl 		_sys_k_event_logger_context_switch
#endif
	/* Get reference to _kernel */
	la 		$t0, _kernel

	/* Get pointer to _kernel.current */
	ld.d 	$t1, $t0, _kernel_offset_to_current

	/*
	 * Save callee-saved registers of current thread
	 * prior to handle context-switching
	 */
	st.d $s0, $t1, _thread_offset_to_s0
	st.d $s1, $t1, _thread_offset_to_s1
	st.d $s2, $t1, _thread_offset_to_s2
	st.d $s3, $t1, _thread_offset_to_s3
	st.d $s4, $t1, _thread_offset_to_s4
	st.d $s5, $t1, _thread_offset_to_s5
	st.d $s6, $t1, _thread_offset_to_s6
	st.d $s7, $t1, _thread_offset_to_s7
	st.d $s8, $t1, _thread_offset_to_s8

	/*
	 * Save stack pointer of current thread and set the default return value
	 * of _Swap to _k_neg_eagain for the thread.
	 */
	st.d $sp, $t1, _thread_offset_to_sp
	la $t2, _k_neg_eagain
	ld.d $t3, $t2, 0x00
	st.d $t3, $t1, _thread_offset_to_swap_return_value

	/* Get next thread to schedule. */
	ld.d $t1, $t0, _kernel_offset_to_ready_q_cache

	/*
	 * Set _kernel.current to new thread loaded in $t1
	 */
	st.d $t1, $t0, _kernel_offset_to_current

	/* Switch to new thread stack */
	ld.d $sp, $t1, _thread_offset_to_sp

	/* Restore callee-saved registers of new thread */
	ld.d $s0, $t1, _thread_offset_to_s0
	ld.d $s1, $t1, _thread_offset_to_s1
	ld.d $s2, $t1, _thread_offset_to_s2
	ld.d $s3, $t1, _thread_offset_to_s3
	ld.d $s4, $t1, _thread_offset_to_s4
	ld.d $s5, $t1, _thread_offset_to_s5
	ld.d $s6, $t1, _thread_offset_to_s6
	ld.d $s7, $t1, _thread_offset_to_s7
	ld.d $s8, $t1, _thread_offset_to_s8

no_reschedule:
#ifdef CONFIG_LOONGARCH_SOC_CONTEXT_SAVE
	bl 		__soc_restore_context
#endif

	csrrd	$t0, CSR_SAVE1
	csrwr	$t0, CSR_PRMD

	ld.d	$t0, $sp, ESF_ERA_OFFSET
	csrwr	$t0, CSR_ERA

	store_load_regs ld.d
	addi.d	$sp, $sp, ESF_STACK_SIZE
	ertn
