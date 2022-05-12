// SPDX-License-Identifier: GPL-2.0-only
/*
 * Copyright 2008 IBM Corp. 
 *
 * Based on arch/powerpc/platforms/pasemi/idle.c: 
 * Copyright (C) 2006-2007 PA Semi, Inc
 *
 * Added by: Jerone Young <jyoung5@us.ibm.com>
 */

#include <linux/of.h>
#include <linux/kernel.h>
#include <asm/machdep.h>

static int mode_spin;

static void ppc44x_idle(void)
{
	unsigned long msr_save;

	msr_save = mfmsr();
	/* set wait state MSR */
	mtmsr(msr_save|MSR_WE|MSR_EE|MSR_CE|MSR_DE);
	isync();
	/* return to initial state */
	mtmsr(msr_save);
	isync();
}

int __init ppc44x_idle_init(void)
{
	if (!mode_spin) {
		/* If we are not setting spin mode 
                   then we set to wait mode */
		ppc_md.power_save = &ppc44x_idle;
	}

	return 0;
}

arch_initcall(ppc44x_idle_init);

static int __init idle_param(char *p)
{ 

	if (!strcmp("spin", p)) {
		mode_spin = 1;
		ppc_md.power_save = NULL;
	}

	return 0;
}

early_param("idle", idle_param);
