/**
 * @file
 * @brief LoongArch public exception handling
 *
 * LoongArch-specific kernel exception handling interface.
 */

#ifndef ZEPHYR_INCLUDE_ARCH_LOONGARCH_EXCEPTION_H_
#define ZEPHYR_INCLUDE_ARCH_LOONGARCH_EXCEPTION_H_

#ifndef _ASMLANGUAGE

#include <zephyr/types.h>
#include <toolchain.h>

#ifdef CONFIG_LOONGARCH_SOC_CONTEXT_SAVE
#include <soc_context.h>
#endif

#ifdef CONFIG_LOONGARCH_SOC_HAS_ISR_STACKING
#include <soc_isr_stacking.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif

/*
 * SoC specific exception frame
 */

#ifdef CONFIG_LOONGARCH_SOC_CONTEXT_SAVE

struct soc_esf {
	SOC_ESF_MEMBERS;
};

#endif


#ifdef CONFIG_EXCEPTION_DEBUG
struct _callee_saved;
typedef struct _callee_saved _callee_saved_t;
#endif


#if defined(CONFIG_LOONGARCH_SOC_HAS_ISR_STACKING)

SOC_ISR_STACKING_ESF_DECLARE;

#else


/*
 * LoongArch Exception Stack Frame
 */

struct arch_esf {

	/* return address */

	unsigned long ra;

	/*
	 * Temporary registers
	 */

	unsigned long t0;
	unsigned long t1;
	unsigned long t2;
	unsigned long t3;
	unsigned long t4;
	unsigned long t5;
	unsigned long t6;
	unsigned long t7;
	unsigned long t8;


	/*
	 * Argument registers
	 */

	unsigned long a0;
	unsigned long a1;
	unsigned long a2;
	unsigned long a3;
	unsigned long a4;
	unsigned long a5;
	unsigned long a6;
	unsigned long a7;


	/*
	 * Saved registers
	 */

	unsigned long s0;
	unsigned long s1;
	unsigned long s2;
	unsigned long s3;
	unsigned long s4;
	unsigned long s5;
	unsigned long s6;
	unsigned long s7;
	unsigned long s8;


	/*
	 * Exception CSRs
	 */

	unsigned long r21;      /* Thread Local Storage */
	unsigned long tp;       /* Thread Pointer */
	unsigned long fp;

	unsigned long era;      /* Exception Return Address */
	unsigned long estat;    /* Exception Status */
	unsigned long crmd;     /* Current Mode Register */
	unsigned long badv;     /* Bad Virtual Address */


#ifdef CONFIG_USERSPACE

	unsigned long sp;       /* user or kernel stack pointer */

#endif


#ifdef CONFIG_EXCEPTION_DEBUG

	_callee_saved_t *csf;

#endif


#ifdef CONFIG_LOONGARCH_SOC_CONTEXT_SAVE

	struct soc_esf soc_context;

#endif

} __aligned(16);

typedef struct arch_esf arch_esf_t;

#endif /* CONFIG_LOONGARCH_SOC_HAS_ISR_STACKING */


#ifdef CONFIG_LOONGARCH_SOC_CONTEXT_SAVE
typedef struct soc_esf soc_esf_t;
#endif

typedef struct arch_esf NANO_ESF;
extern const NANO_ESF _default_esf;
extern FUNC_NORETURN void _NanoFatalErrorHandler(unsigned int reason,
                                                const NANO_ESF *esf);
extern void _SysFatalErrorHandler(unsigned int reason,
                                  const NANO_ESF *esf);

#define _NANO_ERR_CPU_EXCEPTION (0)
#define _NANO_ERR_INVALID_TASK_EXIT (1)
#define _NANO_ERR_STACK_CHK_FAIL (2)
#define _NANO_ERR_ALLOCATION_FAIL (3)
#define _NANO_ERR_SPURIOUS_INT (4)
#define _NANO_ERR_KERNEL_OOPS (5)
#define _NANO_ERR_KERNEL_PANIC (6)

#ifdef __cplusplus
}
#endif

#endif /* _ASMLANGUAGE */

#endif /* ZEPHYR_INCLUDE_ARCH_LOONGARCH_EXCEPTION_H_ */
