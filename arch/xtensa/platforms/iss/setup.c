// SPDX-License-Identifier: GPL-2.0-or-later
/*
 *
 * arch/xtensa/platform-iss/setup.c
 *
 * Platform specific initialization.
 *
 * Authors: Chris Zankel <chris@zankel.net>
 *          Joe Taylor <joe@tensilica.com>
 *
 * Copyright 2001 - 2005 Tensilica Inc.
 * Copyright 2017 Cadence Design Systems Inc.
 */
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/notifier.h>
#include <linux/printk.h>
#include <linux/string.h>

#include <asm/platform.h>
#include <asm/setup.h>

#include <platform/simcall.h>


void platform_halt(void)
{
	pr_info(" ** Called platform_halt() **\n");
	simc_exit(0);
}

void platform_power_off(void)
{
	pr_info(" ** Called platform_power_off() **\n");
	simc_exit(0);
}

void platform_restart(void)
{
	/* Flush and reset the mmu, simulate a processor reset, and
	 * jump to the reset vector. */
	cpu_reset();
	/* control never gets here */
}

static int
iss_panic_event(struct notifier_block *this, unsigned long event, void *ptr)
{
	simc_exit(1);
	return NOTIFY_DONE;
}

static struct notifier_block iss_panic_block = {
	.notifier_call = iss_panic_event,
};

void __init platform_setup(char **p_cmdline)
{
	static void *argv[COMMAND_LINE_SIZE / sizeof(void *)] __initdata;
	static char cmdline[COMMAND_LINE_SIZE] __initdata;
	int argc = simc_argc();
	int argv_size = simc_argv_size();

	if (argc > 1) {
		if (argv_size > sizeof(argv)) {
			pr_err("%s: command line too long: argv_size = %d\n",
			       __func__, argv_size);
		} else {
			int i;

			cmdline[0] = 0;
			simc_argv((void *)argv);

			for (i = 1; i < argc; ++i) {
				if (i > 1)
					strcat(cmdline, " ");
				strcat(cmdline, argv[i]);
			}
			*p_cmdline = cmdline;
		}
	}

	atomic_notifier_chain_register(&panic_notifier_list, &iss_panic_block);
}
