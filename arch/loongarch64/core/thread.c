/*
 * SPDX-License-Identifier: Apache-2.0
 */

#include <kernel.h>
#include <arch/cpu.h>
#include <kernel_structs.h>
#include <wait_q.h>
#include <arch/loongarch64/csr.h>

void _thread_entry_wrapper(_thread_entry_t thread,
			   void *arg1,
			   void *arg2,
			   void *arg3);

/* Default exception stack frame used by common thread entry */
const struct arch_esf _default_esf;

void _new_thread(struct k_thread *thread, char *stack_memory,
	     size_t stack_size, _thread_entry_t thread_func,
	     void *arg1, void *arg2, void *arg3,
	     int priority, unsigned int options)
{
	_ASSERT_VALID_PRIO(priority, thread_func);

	struct arch_esf *stack_init;

	_new_thread_init(thread, stack_memory, stack_size, priority, options);

	/* Initial stack frame for thread */
	stack_init = (struct arch_esf *)
		STACK_ROUND_DOWN(stack_memory + stack_size - sizeof(struct arch_esf));

	/* Setup the initial stack frame */
	stack_init->a0 = (unsigned long)thread_func;
	stack_init->a1 = (unsigned long)arg1;
	stack_init->a2 = (unsigned long)arg2;
	stack_init->a3 = (unsigned long)arg3;
	/*
	 * LoongArch64: CRMD holds global interrupt enable (IE).
	 * Ensure new thread starts with interrupts enabled and PC at wrapper.
	 */
	stack_init->crmd = CRMD_IE;
	stack_init->era = (unsigned long)_thread_entry_wrapper;

	thread->callee_saved.sp = (unsigned long)stack_init;

	thread_monitor_init(thread);
}

