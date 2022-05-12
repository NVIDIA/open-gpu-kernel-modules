// SPDX-License-Identifier: GPL-2.0-or-later
/*
 * Copyright (C) 2000,2001,2002,2003,2004 Broadcom Corporation
 */
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/export.h>
#include <linux/reboot.h>
#include <linux/string.h>

#include <asm/bootinfo.h>
#include <asm/cpu.h>
#include <asm/mipsregs.h>
#include <asm/io.h>
#include <asm/sibyte/sb1250.h>

#include <asm/sibyte/bcm1480_regs.h>
#include <asm/sibyte/bcm1480_scd.h>
#include <asm/sibyte/sb1250_scd.h>

unsigned int sb1_pass;
unsigned int soc_pass;
unsigned int soc_type;
EXPORT_SYMBOL(soc_type);
unsigned int periph_rev;
EXPORT_SYMBOL_GPL(periph_rev);
unsigned int zbbus_mhz;
EXPORT_SYMBOL(zbbus_mhz);

static unsigned int part_type;

static char *soc_str;
static char *pass_str;

static int __init setup_bcm1x80_bcm1x55(void)
{
	int ret = 0;

	switch (soc_pass) {
	case K_SYS_REVISION_BCM1480_S0:
		periph_rev = 1;
		pass_str = "S0 (pass1)";
		break;
	case K_SYS_REVISION_BCM1480_A1:
		periph_rev = 1;
		pass_str = "A1 (pass1)";
		break;
	case K_SYS_REVISION_BCM1480_A2:
		periph_rev = 1;
		pass_str = "A2 (pass1)";
		break;
	case K_SYS_REVISION_BCM1480_A3:
		periph_rev = 1;
		pass_str = "A3 (pass1)";
		break;
	case K_SYS_REVISION_BCM1480_B0:
		periph_rev = 1;
		pass_str = "B0 (pass2)";
		break;
	default:
		printk("Unknown %s rev %x\n", soc_str, soc_pass);
		periph_rev = 1;
		pass_str = "Unknown Revision";
		break;
	}

	return ret;
}

/* Setup code likely to be common to all SiByte platforms */

static int __init sys_rev_decode(void)
{
	int ret = 0;

	switch (soc_type) {
	case K_SYS_SOC_TYPE_BCM1x80:
		if (part_type == K_SYS_PART_BCM1480)
		    soc_str = "BCM1480";
		else if (part_type == K_SYS_PART_BCM1280)
		    soc_str = "BCM1280";
		else
		    soc_str = "BCM1x80";
		ret = setup_bcm1x80_bcm1x55();
		break;

	case K_SYS_SOC_TYPE_BCM1x55:
		if (part_type == K_SYS_PART_BCM1455)
		    soc_str = "BCM1455";
		else if (part_type == K_SYS_PART_BCM1255)
		    soc_str = "BCM1255";
		else
		    soc_str = "BCM1x55";
		ret = setup_bcm1x80_bcm1x55();
		break;

	default:
		printk("Unknown part type %x\n", part_type);
		ret = 1;
		break;
	}

	return ret;
}

void __init bcm1480_setup(void)
{
	uint64_t sys_rev;
	int plldiv;

	sb1_pass = read_c0_prid() & PRID_REV_MASK;
	sys_rev = __raw_readq(IOADDR(A_SCD_SYSTEM_REVISION));
	soc_type = SYS_SOC_TYPE(sys_rev);
	part_type = G_SYS_PART(sys_rev);
	soc_pass = G_SYS_REVISION(sys_rev);

	if (sys_rev_decode()) {
		printk("Restart after failure to identify SiByte chip\n");
		machine_restart(NULL);
	}

	plldiv = G_BCM1480_SYS_PLL_DIV(__raw_readq(IOADDR(A_SCD_SYSTEM_CFG)));
	zbbus_mhz = ((plldiv >> 1) * 50) + ((plldiv & 1) * 25);

	printk("Broadcom SiByte %s %s @ %d MHz (SB-1A rev %d)\n",
		    soc_str, pass_str, zbbus_mhz * 2, sb1_pass);
	printk("Board type: %s\n", get_system_type());
}
