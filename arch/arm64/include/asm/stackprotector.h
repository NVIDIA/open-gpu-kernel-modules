/* SPDX-License-Identifier: GPL-2.0 */
/*
 * GCC stack protector support.
 *
 * Stack protector works by putting predefined pattern at the start of
 * the stack frame and verifying that it hasn't been overwritten when
 * returning from the function.  The pattern is called stack canary
 * and gcc expects it to be defined by a global variable called
 * "__stack_chk_guard" on ARM.  This unfortunately means that on SMP
 * we cannot have a different canary value per task.
 */

#ifndef __ASM_STACKPROTECTOR_H
#define __ASM_STACKPROTECTOR_H

#include <linux/random.h>
#include <linux/version.h>
#include <asm/pointer_auth.h>

extern unsigned long __stack_chk_guard;

/*
 * Initialize the stackprotector canary value.
 *
 * NOTE: this must only be called from functions that never return,
 * and it must always be inlined.
 */
static __always_inline void boot_init_stack_canary(void)
{
#if defined(CONFIG_STACKPROTECTOR)
	unsigned long canary;

	/* Try to get a semi random initial value. */
	get_random_bytes(&canary, sizeof(canary));
	canary ^= LINUX_VERSION_CODE;
	canary &= CANARY_MASK;

	current->stack_canary = canary;
	if (!IS_ENABLED(CONFIG_STACKPROTECTOR_PER_TASK))
		__stack_chk_guard = current->stack_canary;
#endif
	ptrauth_thread_init_kernel(current);
	ptrauth_thread_switch_kernel(current);
	ptrauth_enable();
}

#endif	/* _ASM_STACKPROTECTOR_H */
