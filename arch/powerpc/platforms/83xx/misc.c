// SPDX-License-Identifier: GPL-2.0-or-later
/*
 * misc setup functions for MPC83xx
 *
 * Maintainer: Kumar Gala <galak@kernel.crashing.org>
 */

#include <linux/stddef.h>
#include <linux/kernel.h>
#include <linux/of_platform.h>
#include <linux/pci.h>

#include <asm/debug.h>
#include <asm/io.h>
#include <asm/hw_irq.h>
#include <asm/ipic.h>
#include <sysdev/fsl_soc.h>
#include <sysdev/fsl_pci.h>

#include <mm/mmu_decl.h>

#include "mpc83xx.h"

static __be32 __iomem *restart_reg_base;

static int __init mpc83xx_restart_init(void)
{
	/* map reset restart_reg_baseister space */
	restart_reg_base = ioremap(get_immrbase() + 0x900, 0xff);

	return 0;
}

arch_initcall(mpc83xx_restart_init);

void __noreturn mpc83xx_restart(char *cmd)
{
#define RST_OFFSET	0x00000900
#define RST_PROT_REG	0x00000018
#define RST_CTRL_REG	0x0000001c

	local_irq_disable();

	if (restart_reg_base) {
		/* enable software reset "RSTE" */
		out_be32(restart_reg_base + (RST_PROT_REG >> 2), 0x52535445);

		/* set software hard reset */
		out_be32(restart_reg_base + (RST_CTRL_REG >> 2), 0x2);
	} else {
		printk (KERN_EMERG "Error: Restart registers not mapped, spinning!\n");
	}

	for (;;) ;
}

long __init mpc83xx_time_init(void)
{
#define SPCR_OFFSET	0x00000110
#define SPCR_TBEN	0x00400000
	__be32 __iomem *spcr = ioremap(get_immrbase() + SPCR_OFFSET, 4);
	__be32 tmp;

	tmp = in_be32(spcr);
	out_be32(spcr, tmp | SPCR_TBEN);

	iounmap(spcr);

	return 0;
}

void __init mpc83xx_ipic_init_IRQ(void)
{
	struct device_node *np;

	/* looking for fsl,pq2pro-pic which is asl compatible with fsl,ipic */
	np = of_find_compatible_node(NULL, NULL, "fsl,ipic");
	if (!np)
		np = of_find_node_by_type(NULL, "ipic");
	if (!np)
		return;

	ipic_init(np, 0);

	of_node_put(np);

	/* Initialize the default interrupt mapping priorities,
	 * in case the boot rom changed something on us.
	 */
	ipic_set_default_priority();
}

static const struct of_device_id of_bus_ids[] __initconst = {
	{ .type = "soc", },
	{ .compatible = "soc", },
	{ .compatible = "simple-bus" },
	{ .compatible = "gianfar" },
	{ .compatible = "gpio-leds", },
	{ .type = "qe", },
	{ .compatible = "fsl,qe", },
	{},
};

int __init mpc83xx_declare_of_platform_devices(void)
{
	of_platform_bus_probe(NULL, of_bus_ids, NULL);
	return 0;
}

#ifdef CONFIG_PCI
void __init mpc83xx_setup_pci(void)
{
	struct device_node *np;

	for_each_compatible_node(np, "pci", "fsl,mpc8349-pci")
		mpc83xx_add_bridge(np);
	for_each_compatible_node(np, "pci", "fsl,mpc8314-pcie")
		mpc83xx_add_bridge(np);
}
#endif

void __init mpc83xx_setup_arch(void)
{
	if (ppc_md.progress)
		ppc_md.progress("mpc83xx_setup_arch()", 0);

	if (!__map_without_bats) {
		phys_addr_t immrbase = get_immrbase();
		int immrsize = IS_ALIGNED(immrbase, SZ_2M) ? SZ_2M : SZ_1M;
		unsigned long va = fix_to_virt(FIX_IMMR_BASE);

		setbat(-1, va, immrbase, immrsize, PAGE_KERNEL_NCG);
		update_bats();
	}
}

int machine_check_83xx(struct pt_regs *regs)
{
	u32 mask = 1 << (31 - IPIC_MCP_WDT);

	if (!(regs->msr & SRR1_MCE_MCP) || !(ipic_get_mcp_status() & mask))
		return machine_check_generic(regs);
	ipic_clear_mcp_status(mask);

	if (debugger_fault_handler(regs))
		return 1;

	die("Watchdog NMI Reset", regs, 0);

	return 1;
}
