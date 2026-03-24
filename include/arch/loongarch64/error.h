/**
 * @file
 * @brief LOONGARCH public error handling
 *
 * LOONGARCH-specific kernel error handling interface. Included by loongarch/arch.h.
 */

#ifndef ZEPHYR_INCLUDE_ARCH_LOONGARCH_ERROR_H_
#define ZEPHYR_INCLUDE_ARCH_LOONGARCH_ERROR_H_

#include <zephyr/arch/loongarch64/syscall.h>
#include <zephyr/arch/exception.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

#ifdef CONFIG_USERSPACE

#define ARCH_EXCEPT(reason_p)	do {			\
		if (k_is_user_context()) {		\
			arch_syscall_invoke1(reason_p,	\
				K_SYSCALL_USER_FAULT);	\
		} else {				\
			compiler_barrier();		\
			arch_syscall_invoke1(reason_p,	\
				LARCH_SYSCALL_RUNTIME_EXCEPT);\
		}					\
		CODE_UNREACHABLE; /* LCOV_EXCL_LINE */	\
	} while (false)
#else
#define ARCH_EXCEPT(reason_p) \
	arch_syscall_invoke1(reason_p, LARCH_SYSCALL_RUNTIME_EXCEPT)
#endif

__syscall void user_fault(unsigned int reason);

#include <zephyr/syscalls/error.h>

#ifdef __cplusplus
}
#endif

#endif /* ZEPHYR_INCLUDE_ARCH_LOONGARCH_ERROR_H_ */
