// SPDX-License-Identifier: GPL-2.0
/*
 * Reset a DECstation machine.
 *
 * Copyright (C) 199x  the Anonymous
 * Copyright (C) 2001, 2002, 2003  Maciej W. Rozycki
 */
#include <linux/interrupt.h>
#include <linux/linkage.h>

#include <asm/addrspace.h>

typedef void __noreturn (* noret_func_t)(void);

static inline void __noreturn back_to_prom(void)
{
	noret_func_t func = (void *)CKSEG1ADDR(0x1fc00000);

	func();
}

void __noreturn dec_machine_restart(char *command)
{
	back_to_prom();
}

void __noreturn dec_machine_halt(void)
{
	back_to_prom();
}

void __noreturn dec_machine_power_off(void)
{
    /* DECstations don't have a software power switch */
	back_to_prom();
}

irqreturn_t dec_intr_halt(int irq, void *dev_id)
{
	dec_machine_halt();
}
