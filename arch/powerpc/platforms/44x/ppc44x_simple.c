// SPDX-License-Identifier: GPL-2.0-only
/*
 * Generic PowerPC 44x platform support
 *
 * Copyright 2008 IBM Corporation
 *
 * This implements simple platform support for PowerPC 44x chips.  This is
 * mostly used for eval boards or other simple and "generic" 44x boards.  If
 * your board has custom functions or hardware, then you will likely want to
 * implement your own board.c file to accommodate it.
 */

#include <asm/machdep.h>
#include <asm/pci-bridge.h>
#include <asm/ppc4xx.h>
#include <asm/prom.h>
#include <asm/time.h>
#include <asm/udbg.h>
#include <asm/uic.h>

#include <linux/init.h>
#include <linux/of_platform.h>

static const struct of_device_id ppc44x_of_bus[] __initconst = {
	{ .compatible = "ibm,plb4", },
	{ .compatible = "ibm,opb", },
	{ .compatible = "ibm,ebc", },
	{ .compatible = "simple-bus", },
	{},
};

static int __init ppc44x_device_probe(void)
{
	of_platform_bus_probe(NULL, ppc44x_of_bus, NULL);

	return 0;
}
machine_device_initcall(ppc44x_simple, ppc44x_device_probe);

/* This is the list of boards that can be supported by this simple
 * platform code.  This does _not_ mean the boards are compatible,
 * as they most certainly are not from a device tree perspective.
 * However, their differences are handled by the device tree and the
 * drivers and therefore they don't need custom board support files.
 *
 * Again, if your board needs to do things differently then create a
 * board.c file for it rather than adding it to this list.
 */
static char *board[] __initdata = {
	"amcc,arches",
	"amcc,bamboo",
	"apm,bluestone",
	"amcc,glacier",
	"ibm,ebony",
	"amcc,eiger",
	"amcc,katmai",
	"amcc,rainier",
	"amcc,redwood",
	"amcc,sequoia",
	"amcc,taishan",
	"amcc,yosemite",
	"mosaixtech,icon"
};

static int __init ppc44x_probe(void)
{
	int i = 0;

	for (i = 0; i < ARRAY_SIZE(board); i++) {
		if (of_machine_is_compatible(board[i])) {
			pci_set_flags(PCI_REASSIGN_ALL_RSRC);
			return 1;
		}
	}

	return 0;
}

define_machine(ppc44x_simple) {
	.name = "PowerPC 44x Platform",
	.probe = ppc44x_probe,
	.progress = udbg_progress,
	.init_IRQ = uic_init_tree,
	.get_irq = uic_get_irq,
	.restart = ppc4xx_reset_system,
	.calibrate_decr = generic_calibrate_decr,
};
